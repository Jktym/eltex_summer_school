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

static int parse_contact_from_line(const char *line, Contact *c) {
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

void init_phonebook(Phonebook *pb) {
    if (pb == NULL) return;
    
    pb->count = 0;
    memset(pb->contacts, 0, sizeof(pb->contacts));
    
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        return;
    }
    
    printf("Загрузка контактов из файла %s...\n", CONTACTS_FILE);
    
    char line[1024];
    int loaded = 0;
    int errors = 0;
    
    while (fgets(line, sizeof(line), file) != NULL && pb->count < PHONEBOOK_SIZE) {
        if (strlen(line) <= 1) continue;
        
        Contact c = {0};
        if (parse_contact_from_line(line, &c)) {
            pb->contacts[pb->count] = c;
            pb->contacts[pb->count].id = pb->count + 1;
            pb->count++;
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
}

void save_phonebook(const Phonebook *pb) {
    if (pb == NULL) return;
    
    FILE *file = fopen(CONTACTS_FILE, "w");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл %s для записи.\n", CONTACTS_FILE);
        return;
    }
    
    for (int i = 0; i < pb->count; i++) {
        const Contact *c = &pb->contacts[i];
        fprintf(file, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                c->id,
                c->name,
                c->second_name,
                c->surname,
                c->work,
                c->position,
                c->mob_phone,
                c->work_phone,
                c->email,
                c->link);
    }
    
    fclose(file);
}

void safe_read(const char *prompt, char *input, int max_len) {
    printf("%s", prompt);
    if(fgets(input, max_len, stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
    } else {
        input[0] = '\0';
    }
}

Contact input_contact() {
    Contact c;
    memset(&c, 0, sizeof(Contact));

    printf("\n--- Ввод нового контакта ---\n");
    printf("(Обязательные поля помечены *)\n\n");

    safe_read("* Имя: ",            c.name,        MAX_NAME_L);
    safe_read("* Фамилия: ",        c.surname,     MAX_NAME_L);
    safe_read("  Отчество: ",       c.second_name, MAX_NAME_L);
    safe_read("  Место работы: ",   c.work,        MAX_WORK_L);
    safe_read("  Должность: ",      c.position,    MAX_WORK_L);
    safe_read("  Моб. телефон: ",   c.mob_phone,   MAX_PHONE_L);
    safe_read("  Раб. телефон: ",   c.work_phone,  MAX_PHONE_L);
    safe_read("  Email: ",          c.email,       MAX_EMAIL_L);
    safe_read("  Ссылка на соц сети: ",         c.link,        MAX_LINK_L);

    return c;
}

int add_contact(Phonebook *pb, const Contact *new_contact) {
    if (pb == NULL || new_contact == NULL) return -1;
    if (pb->count >= PHONEBOOK_SIZE) return -2;
    if(strlen(new_contact->name) == 0 || strlen(new_contact->surname) == 0) return -3;
    pb->contacts[pb->count] = *new_contact;
    pb->contacts[pb->count].id = pb->count + 1;
    pb->count++;
    return 0;
}

int find_contact_with_id(Phonebook *pb, int selected_id) {
    if (pb == NULL) return 0;
    for (int i = 0; i < pb->count; i++) {
        if (pb->contacts[i].id == selected_id) return 1;
    }
    return 0;
}

int find_contact_with_field(const Phonebook *pb, Contact *results, const char *info) {
    if (pb == NULL || results == NULL || info == NULL || strlen(info) == 0) {
        return 0;
    }

    int found_count = 0;

    for (int i = 0; i < pb->count; i++) {
        const Contact *c = &pb->contacts[i]; 

        if (strstr(c->name, info) != NULL ||
            strstr(c->surname, info) != NULL ||
            strstr(c->second_name, info) != NULL ||
            strstr(c->work, info) != NULL ||
            strstr(c->position, info) != NULL ||
            strstr(c->mob_phone, info) != NULL ||
            strstr(c->work_phone, info) != NULL ||
            strstr(c->email, info) != NULL ||
            strstr(c->link, info) != NULL) {
            
            results[found_count] = *c; 
            found_count++;
        }
    }

    return found_count;
}

int delete_contact(Phonebook *pb, int index) {
    if (pb == NULL || index < 0 || index >= pb->count) return 0;
    
    for (int i = index; i < pb->count - 1; i++) {
        pb->contacts[i] = pb->contacts[i + 1];
    }
    
    memset(&pb->contacts[pb->count - 1], 0, sizeof(Contact));
    pb->count--;
    
    for (int i = 0; i < pb->count; i++) {
        pb->contacts[i].id = i + 1;
    }
    
    return 1;
}

void print_contact_info(const Contact *c) {
    printf("Фамилия:    %s\n", c->surname);
    printf("Имя:        %s\n", c->name);
    printf("Отчество:   %s\n", c->second_name[0] ? c->second_name : "(не указано)");
    printf("Место работы: %s\n", c->work[0] ? c->work : "(не указано)");
    printf("Должность:  %s\n", c->position[0] ? c->position : "(не указана)");
    printf("Моб. телефон: %s\n", c->mob_phone[0] ? c->mob_phone : "(не указан)");
    printf("Раб. телефон: %s\n", c->work_phone[0] ? c->work_phone : "(не указан)");
    printf("Email:      %s\n", c->email[0] ? c->email : "(не указан)");
    printf("Ссылка на соц. сети:     %s\n", c->link[0] ? c->link : "(не указана)");
    printf("=====================================\n");
}

void edit_contact(Phonebook *pb, int current_id, int field) {
    if (pb == NULL) {
        printf("Ошибка редактирования.\n");
    }
    printf("Введите новое значение\n");
    switch (field) {
        case 1:
            safe_read(" Имя: ", pb->contacts[current_id].name, MAX_NAME_L);
            break;
        case 2:
            safe_read(" Фамилия: ", pb->contacts[current_id].surname,MAX_NAME_L);
            break;
        case 3:
            safe_read("  Отчество: ", pb->contacts[current_id].second_name, MAX_NAME_L);
            break;
        case 4:
            safe_read("  Место работы: ",pb->contacts[current_id].work, MAX_WORK_L);
            break;
        case 5:
            safe_read("  Должность: ", pb->contacts[current_id].position, MAX_WORK_L);
            break;
        case 6:
            safe_read("  Моб. телефон: ", pb->contacts[current_id].mob_phone, MAX_PHONE_L);
            break;
        case 7:
            safe_read("  Раб. телефон: ", pb->contacts[current_id].work_phone, MAX_PHONE_L);
            break;
        case 8:
            safe_read("  Email: ", pb->contacts[current_id].email, MAX_EMAIL_L);
            break;
        case 9:
            safe_read("  Ссылка на соц сети: ", pb->contacts[current_id].link, MAX_LINK_L);
            break;
    }
}