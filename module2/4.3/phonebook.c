#include "phonebook.h"

static void extract_field(const char **str, char *buffer, size_t buf_size) {
    const char *start = *str;
    const char *end = strchr(start, ';');
    
    if (end != NULL) {
        size_t len = end - start;
        if (len >= buf_size) len = buf_size - 1;
        strncpy(buffer, start, len);
        buffer[len] = '\0';
        *str = end + 1;
    } else {
        strncpy(buffer, start, buf_size - 1);
        buffer[buf_size - 1] = '\0';
        *str = NULL;
    }
}

static int parse_contact_from_line(const char *line, Data *c) {
    if (line == NULL || strlen(line) == 0) return 0;
    
    char buffer[1024];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    
    const char *ptr = buffer;
    char id_str[20];
    
    extract_field(&ptr, id_str, sizeof(id_str));
    extract_field(&ptr, c->name, MAX_NAME_L);
    extract_field(&ptr, c->second_name, MAX_NAME_L);
    extract_field(&ptr, c->surname, MAX_NAME_L);
    extract_field(&ptr, c->work, MAX_WORK_L);
    extract_field(&ptr, c->position, MAX_WORK_L);
    extract_field(&ptr, c->mob_phone, MAX_PHONE_L);
    extract_field(&ptr, c->work_phone, MAX_PHONE_L);
    extract_field(&ptr, c->email, MAX_EMAIL_L);
    extract_field(&ptr, c->link, MAX_LINK_L);
    
    if (strlen(c->name) == 0 || strlen(c->surname) == 0) {
        return 0;
    }
    
    return 1;
}


static void reindex_tree(Contact *contact, int *current_id) {
    if (!contact) return;
    reindex_tree(contact->left, current_id);
    contact->data->id = (*current_id)++;
    reindex_tree(contact->right, current_id);
}

static void reindex_phonebook(Phonebook *pb) {
    if (!pb || !pb->root) return;
    int starting_id = 1;
    reindex_tree(pb->root, &starting_id);
}

static void store_nodes(Contact *contact, Contact **arr, int *idx) {
    if (!contact) return;
    store_nodes(contact->left, arr, idx);
    arr[*idx] = contact;
    (*idx)++;
    store_nodes(contact->right, arr, idx);
}

static Contact *build_balanced_tree(Contact **arr, int start, int end) {
    if (start > end) return NULL;
    int mid = (start + end) / 2;
    Contact *root = arr[mid];
    
    root->left = build_balanced_tree(arr, start, mid - 1);
    root->right = build_balanced_tree(arr, mid + 1, end);
    return root;
}

static void balance_phonebook(Phonebook *pb) {
    if (!pb || !pb->root || pb->count == 0) return;
    
    Contact **arr = malloc(pb->count * sizeof(Contact*));
    int idx = 0;
    
    store_nodes(pb->root, arr, &idx);
    pb->root = build_balanced_tree(arr, 0, idx - 1);
    
    free(arr);
    reindex_phonebook(pb); 
}

static void insert_contact(Contact *contact, Contact *new_node) {
    if (strcmp(new_node->data->surname, contact->data->surname) <= 0) {
        if (!contact->left) contact->left = new_node;
        else insert_contact(contact->left, new_node);
    } else {
        if (!contact->right) contact->right = new_node;
        else insert_contact(contact->right, new_node);
    }
}

Phonebook *add_contact(Phonebook *pb, Data new_data) {
    Contact *new_contact = malloc(sizeof(Contact));
    Data *data = malloc(sizeof(Data));
    
    *data = new_data; 
    new_contact->data = data;
    new_contact->left = NULL;
    new_contact->right = NULL;
    
    if (pb->root == NULL) {
        pb->root = new_contact;
    } else {
        insert_contact(pb->root, new_contact);
    }
    
    pb->count++;
    
    if (pb->count % 5 == 0) {
        balance_phonebook(pb);
    } else {
        reindex_phonebook(pb);
    }
    
    return pb;
}

static Contact *delete_node_by_ptr(Contact *contact, Contact *target) {
    if (!contact) return NULL;
    
    if (contact == target) {
        if (!contact->left) {
            Contact *right = contact->right;
            free(contact->data);
            free(contact);
            return right;
        } else if (!contact->right) {
            Contact *left = contact->left;
            free(contact->data);
            free(contact);
            return left;
        }
        
        Contact *min_right = contact->right;
        while (min_right->left) min_right = min_right->left;
        
        Data *temp = contact->data;
        contact->data = min_right->data;
        min_right->data = temp;
        
        contact->right = delete_node_by_ptr(contact->right, min_right);
        return contact;
    }
    
    int cmp = strcmp(target->data->surname, contact->data->surname);
    if (cmp <= 0) contact->left = delete_node_by_ptr(contact->left, target);
    if (cmp >= 0) contact->right = delete_node_by_ptr(contact->right, target);
    
    return contact;
}

Phonebook *delete_contact(Phonebook *pb, int choice_id) {
    if(pb->root == NULL) {
        printf("Список пуст.\n");
        return pb;
    }

    Contact *target = get_contact(pb, choice_id);
    if (!target) {
        printf("Ошибка, контакт не найден.\n");
        return pb;
    }

    pb->root = delete_node_by_ptr(pb->root, target);
    pb->count--;
    
    balance_phonebook(pb);
    
    return pb;
}

Phonebook *init_phonebook() {
    Phonebook *pb = malloc(sizeof(Phonebook));
    pb->root = NULL;
    pb->count = 0;
    
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        return pb;
    }
    
    printf("Загрузка контактов из файла %s...\n", CONTACTS_FILE);
    
    char line[1024];
    int loaded = 0;
    int errors = 0;
    
    while (fgets(line, sizeof(line), file) != NULL && pb->count < PHONEBOOK_SIZE) {
        if (strlen(line) <= 1) continue;
        
        Data data = {0};
        if (parse_contact_from_line(line, &data)) {
            Contact *new_contact = malloc(sizeof(Contact));
            Data *d = malloc(sizeof(Data));
            *d = data;
            new_contact->data = d;
            new_contact->left = new_contact->right = NULL;
            
            if (!pb->root) pb->root = new_contact;
            else insert_contact(pb->root, new_contact);
            
            pb->count++;
            loaded++;
        } else {
            errors++;
        }
    }
    fclose(file);
    
    balance_phonebook(pb);
    
    if (loaded > 0) printf("Загружено контактов: %d\n", loaded);
    if (errors > 0) printf("Пропущено некорректных строк: %d\n", errors);
    
    return pb;
}

static void save_tree(Contact *root, FILE *file) {
    if (!root) return;
    save_tree(root->left, file);
    fprintf(file, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
            root->data->id, root->data->name, root->data->second_name,
            root->data->surname, root->data->work, root->data->position,
            root->data->mob_phone, root->data->work_phone,
            root->data->email, root->data->link);
    save_tree(root->right, file);
}

void save_phonebook(const Phonebook *pb) {
    if (pb == NULL || pb->root == NULL) return;
    
    FILE *file = fopen(CONTACTS_FILE, "w");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл %s для записи.\n", CONTACTS_FILE);
        return;
    }
    
    save_tree(pb->root, file);
    fclose(file);
}

void fill_contact_field(Data *c, char *input, int field) {
    if (c == NULL) return;
    switch(field) {
        case name_field: strncpy(c->name, input, MAX_NAME_L); break;
        case surname_field: strncpy(c->surname, input, MAX_NAME_L); break;
        case second_name_field: strncpy(c->second_name, input, MAX_NAME_L); break;
        case work_field: strncpy(c->work, input, MAX_WORK_L); break;
        case position_field: strncpy(c->position, input, MAX_WORK_L); break;
        case mob_phone_field: strncpy(c->mob_phone, input, MAX_PHONE_L); break;
        case work_phone_field: strncpy(c->work_phone, input, MAX_PHONE_L); break;
        case email_field: strncpy(c->email, input, MAX_EMAIL_L); break;
        case link_field: strncpy(c->link, input, MAX_LINK_L); break;
    }
}

static void search_tree(Contact *root, Contact **results, int *found_count, const char *info) {
    if (!root) return;
    
    search_tree(root->left, results, found_count, info);
    
    if(strstr(root->data->name, info) != NULL ||
       strstr(root->data->surname, info) != NULL ||
       strstr(root->data->second_name, info) != NULL ||
       strstr(root->data->work, info) != NULL ||
       strstr(root->data->position, info) != NULL ||
       strstr(root->data->mob_phone, info) != NULL ||
       strstr(root->data->work_phone, info) != NULL ||
       strstr(root->data->email, info) != NULL ||
       strstr(root->data->link, info) != NULL) {
        
        results[*found_count] = root;
        (*found_count)++;
    }
    
    search_tree(root->right, results, found_count, info);
}

int find_contact_with_field(const Phonebook *pb, Contact **results, const char *info) {
    if (pb == NULL || pb->root == NULL || results == NULL || info == NULL || strlen(info) == 0) return 0;
    int found_count = 0;
    search_tree(pb->root, results, &found_count, info);
    return found_count;
}

static void print_tree(Contact *root) {
    if (!root) return;
    print_tree(root->left);
    printf("%d. %s %s\n", root->data->id, root->data->surname, root->data->name);
    print_tree(root->right);
}

void print_contact_list(Phonebook *pb) {
    if(pb->root == NULL) {
        printf("Список пуст.\n");
        return;
    }
    print_tree(pb->root);
}

void print_contact_info(const Contact *c) {
    printf("Фамилия:    %s\n", c->data->surname);
    printf("Имя:        %s\n", c->data->name);
    printf("Отчество:   %s\n", c->data->second_name[0] ? c->data->second_name : "(не указано)");
    printf("Место работы: %s\n", c->data->work[0] ? c->data->work : "(не указано)");
    printf("Должность:  %s\n", c->data->position[0] ? c->data->position : "(не указана)");
    printf("Моб. телефон: %s\n", c->data->mob_phone[0] ? c->data->mob_phone : "(не указан)");
    printf("Раб. телефон: %s\n", c->data->work_phone[0] ? c->data->work_phone : "(не указан)");
    printf("Email:      %s\n", c->data->email[0] ? c->data->email : "(не указан)");
    printf("Ссылка на соц. сети:     %s\n", c->data->link[0] ? c->data->link : "(не указана)");
    printf("=====================================\n");
}

static Contact *get_contact_rec(Contact *root, int id) {
    if (!root) return NULL;
    Contact *left_res = get_contact_rec(root->left, id);
    if (left_res) return left_res;
    
    if (root->data->id == id) return root;
    
    return get_contact_rec(root->right, id);
}

Contact *get_contact(Phonebook *pb, int choice_id) {
    if (pb->root == NULL) {
        printf("Список контактов пуст.\n");
        return NULL;
    }
    return get_contact_rec(pb->root, choice_id);
}

int get_contact_id(Contact *c) {
    return c->data->id;
}

static void free_tree(Contact *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root->data);
    free(root);
}

void free_phonebook(Phonebook *pb) {
    if(pb->root == NULL) return;
    free_tree(pb->root);
    pb->root = NULL;
    pb->count = 0;
}

static void print_tree_visual_rec(Contact *root, int level) {
    if (root == NULL) {
        return;
    }
    print_tree_visual_rec(root->right, level + 1);

    for (int i = 0; i < level; i++) {
        printf("        "); 
    }

    printf("---> [%d] %s\n", root->data->id, root->data->surname);

    print_tree_visual_rec(root->left, level + 1);
}

void display_tree_visual(Phonebook *pb) {
    if (pb == NULL || pb->root == NULL) {
        printf("Дерево пусто.\n");
        return;
    }
    printf("\n=== СТРУКТУРА ДЕРЕВА (Корень слева, правые узлы сверху) ===\n");
    print_tree_visual_rec(pb->root, 0);
    printf("===========================================================\n");
}