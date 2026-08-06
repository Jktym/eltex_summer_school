#include "menu.h"

enum menu_items {
    print_contacts = 1,
    add_contact = 2,
    find_contact = 3,
    exit = 4
};

void menu() {
    while(1) {
        const char main_menu[] = {"------ГЛАВНОЕ МЕНЮ------\n1.Вывести список контактов\n2.Добавить контакт\n3.Поиск контакта\n0.Выход"};
        printf("%s", main_menu);
        char choice;
        scanf("%c", &choice);
        switch (choice) {
            case print_contacts: 
                printf("%s", get_all_contacts());
                break;
            case add_contact:
                Contact new_contact = input_contact()
        }
    }
}