#include "phonebook.h"

static void safe_read(const char *prompt, char *input, int max_len) {
    printf("%s", prompt);
    if(fgets(input, max_len, stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
    } else {
        input[0] = '\0';
    }
}

void init_phonebook(Phonebook *pb) {
    if (pb == NULL) return;

    pb->count = 0;
    memset(pb->contacts, 0, sizeof(pb->contacts));
}

Contact input_contact(void) {
    Contact c;
    memset(&c, 0, sizeof(Contact)); // обнуляем от мусора

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
    safe_read("  Ссылка: ",         c.link,        MAX_LINK_L);

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

int delete_contact (Phonebook *pb, int id) {
    if (pb->contacts[id].id != id) return -1;

    for (int i = id; i < pb->count; i++) {
        pb->contacts[i] = pb->contacts[i + 1];
        pb->contacts[i].id = i + 1;
    }

    memset(&pb-contacts[pb-count - 1], 0, sizeof(Contact));
    pb-count--;
}

int edit_contact(Phonebook *pb, int id) {
    
}