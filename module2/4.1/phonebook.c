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

static void reindex_phonebook(Phonebook *pb) {
    if (pb == NULL || pb->head == NULL) return;
    
    Contact *current = pb->head;
    int current_id = 1;
    
    do {
        current->data->id = current_id++;
        current = current->next;
    } while (current != pb->head);
}

Phonebook *add_contact(Phonebook *pb, Data new_data) {
    Contact *new_contact = malloc(sizeof(Contact));
    Data *data = malloc(sizeof(Data));
    
    *data = new_data; 
    new_contact->data = data;
    new_contact->next = NULL;
    
    pb->count++;
    
    Contact *current = pb->head;
    if(current == NULL) {
        pb->head = new_contact; 
        new_contact->prev = new_contact;
        new_contact->next = new_contact;
        new_contact->data->id = 1; 
        return pb;
    }
    
    int inserted = 0;
    do {
        if(strcmp(new_data.surname, current->data->surname) <= 0) {
            new_contact->next = current;
            new_contact->prev = current->prev;
            current->prev->next = new_contact;
            current->prev = new_contact;
            if(current == pb->head) pb->head = new_contact;
            inserted = 1;
            break;
        }
        current = current->next;
    } while(current != pb->head);
    
    if (!inserted) {
        new_contact->next = pb->head;
        new_contact->prev = pb->head->prev;
        pb->head->prev->next = new_contact;
        pb->head->prev = new_contact;
    }
    
    reindex_phonebook(pb);
    
    return pb;
}

Phonebook *delete_contact(Phonebook *pb, int choice_id) {
    Contact *current = pb->head;

    if(pb->head == NULL) {
        printf("Список пуст.\n");
        return pb;
    }

    do {
        if (current->data->id == choice_id) {
            if(current->next == current->prev) { 
                free(current->data); 
                free(current);       
                pb->head = NULL;
                pb->count--;
                return pb;
            }
            
            current->next->prev = current->prev;
            current->prev->next = current->next;
            if(current == pb->head) pb->head = pb->head->next;
            
            free(current->data); 
            free(current);       
            pb->count--;
            
            reindex_phonebook(pb);
            
            return pb;
        }
        current = current->next;
    } while(current != pb->head);

    printf("Ошибка, контакт не найден.\n");
    return pb;
}

Phonebook *init_phonebook() {
    Phonebook *pb = malloc(sizeof(Phonebook));
    pb->head = NULL;
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
            pb = add_contact(pb, data);
            loaded++;
        } else {
            errors++;
        }
    }
    
    fclose(file);
    
    if (loaded > 0) {
        printf("Загружено контактов: %d\n", loaded);
    }
    if (errors > 0) {
        printf("Пропущено некорректных строк: %d\n", errors);
    }
    return pb;
}

void save_phonebook(const Phonebook *pb) {
    if (pb == NULL || pb->head == NULL) return;
    
    FILE *file = fopen(CONTACTS_FILE, "w");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл %s для записи.\n", CONTACTS_FILE);
        return;
    }
    
    Contact *current = pb->head;

    do {
        fprintf(file, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                current->data->id,
                current->data->name,
                current->data->second_name,
                current->data->surname,
                current->data->work,
                current->data->position,
                current->data->mob_phone,
                current->data->work_phone,
                current->data->email,
                current->data->link);
    } while (current != pb->head);
    
    fclose(file);
}

void fill_contact_field(Data *c, char *input, int field) {
    if (c == NULL) return;
    switch(field) {
        case name_field:
            strncpy(c->name, input, MAX_NAME_L);
            break;
        case surname_field:
            strncpy(c->surname, input, MAX_NAME_L);
            break;
        case second_name_field:
            strncpy(c->second_name, input, MAX_NAME_L);
            break;
        case work_field:
            strncpy(c->work, input, MAX_WORK_L);
            break;
        case position_field:
            strncpy(c->position, input, MAX_WORK_L);
            break;
        case mob_phone_field:
            strncpy(c->mob_phone, input, MAX_PHONE_L);
            break;
        case work_phone_field:
            strncpy(c->work_phone, input, MAX_PHONE_L);
            break;
        case email_field:
            strncpy(c->email, input, MAX_EMAIL_L);
            break;
        case link_field:
            strncpy(c->link, input, MAX_LINK_L);
            break;
    }
}

int find_contact_with_field(const Phonebook *pb, Contact **results, const char *info) {
    if (pb == NULL || pb->head == NULL || results == NULL || info == NULL || strlen(info) == 0) {
        return 0;
    }

    int found_count = 0;
    Contact *current = pb->head;
    do {
        if(strstr(current->data->name, info) != NULL ||
        strstr(current->data->surname, info) != NULL ||
        strstr(current->data->second_name, info) != NULL ||
        strstr(current->data->work, info) != NULL ||
        strstr(current->data->position, info) != NULL ||
        strstr(current->data->mob_phone, info) != NULL ||
        strstr(current->data->work_phone, info) != NULL ||
        strstr(current->data->email, info) != NULL ||
        strstr(current->data->link, info) != NULL) {
            results[found_count] = current;
            found_count++;
        }
        current = current->next;
    } while(current != pb->head);

    return found_count;
}

void print_contact_list(Phonebook *pb) {
    Contact *current = pb->head;
    if(pb->head == NULL) {
        printf("Список пуст.\n");
        return;
    }
    do {
        printf("%d. %s %s\n", current->data->id, current->data->surname, current->data->name);
        current = current->next;
    } while(current != pb-> head);
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

Contact *get_contact(Phonebook *pb, int choice_id) {
    if (pb->head == NULL) {
        printf("Список контактов пуст.\n");
        return NULL;
    }
    Contact *current = pb->head; 
    do {
        if(current->data->id == choice_id) return current;
        current = current->next;
    } while(current != pb->head);
    return NULL;
}

int get_contact_id(Contact *c) {
    return c->data->id;
}

void free_phonebook(Phonebook *pb) {
    Contact *current = pb->head;
    Contact *contact_for_free;

    if(pb->head == NULL) return;

    do {
        contact_for_free = current;
        current = current->next;
        free(contact_for_free->data);
        free(contact_for_free);
    } while(current != pb->head);
    pb->head = NULL;
}