#include "menu.h"


void contact_menu(Phonebook *pb, int choice_id) {
    Contact *c = &pb->contacts[choice_id];
    if (c == NULL) {
        printf("Ошибка, контакт не найден!\n");
    }
    
    while(1) {
        print_contact_info(&pb->contacts[choice_id]);
        printf("Выберите действие:\n1. Редактировать контакт\n2. Удалить контакт\n0. Назад\n");
        int choice;
        scanf("%d", &choice);
        switch(choice) {
            case edit_selected_contact:
                printf("Выберите поле для редактирования:\n1. Имя\n2. Фамилия\n...");
                int field;
                scanf("%d", &field);
                if (field > 0 && field < 10) {
                    edit_contact(pb, choice_id, field);
                    save_phonebook(pb); 
                    } else {
                    printf("Нет такого поля.\n");
                }
                break;
            case delete_selected_contact: {
                int result = delete_contact(pb, choice_id);
                if (result) {
                    printf("Контакт успешно удалён.\n");
                    save_phonebook(pb);
                    return;
                } else {
                    printf("Не удалось удалить контакт.\n");
                }
                break;
            }
            case back:
                return;
            default:
                printf("Нет такого пункта\n");
        }
        int c;
        while((c = getchar()) != '\n' && c != EOF);
    }


}

void menu(Phonebook *pb) {
    while(1) {
        const char main_menu[] = {"------ГЛАВНОЕ МЕНЮ------\n1.Вывести список контактов\n2.Добавить контакт\n3.Поиск контакта\n0.Выход\n"};
        printf("%s", main_menu);
        int choice;
        scanf("%d", &choice);
        int c;
        while((c = getchar()) != '\n' && c != EOF);
        switch (choice) {
            case print_contacts:
                if(pb->count == 0) {
                    printf("Телефоная книга пуста.\n");
                } else {
                    printf("====СПИСОК КОНТАКТОВ====\n");
                    for (int i = 0; i < pb->count; i++) {
                        printf("%d. %s %s\n", i+1, pb->contacts[i].name, pb->contacts[i].surname);
                    }
                    printf("Введите номер контакта для выбора (0 для возврата в главное меню).\n");
                    int choice_id;
                    scanf("%d", &choice_id);
                    if (!choice_id) break;
                    if(choice_id < 1 || choice_id > pb->count) {
                        printf("Контакта с таким id не существует.\n");
                        break;
                    }
                    contact_menu(pb, choice_id - 1);
                }
                break;
            case add_new_contact: {
                int c;
                while ((c = getchar()) != '\n' && c != EOF); // Чистим буфер
                
                Contact new_contact = input_contact();
                int result = add_contact(pb, &new_contact);
                
                if (result == 0) {
                    printf("Контакт успешно добавлен.\n");
                    save_phonebook(pb);
                } else if (result == -1) {
                    printf("Произошла ошибка! Не удалось добавить контакт.\n");
                } else if (result == -2) {
                    printf("Телефонная книга заполнена.\n");
                } else if (result == -3) {
                    printf("Не заполнены обязательные поля. Не удалось добавить контакт.\n");
                }
                break;
            }
            case find_contact: {
                printf("\n--- Поиск контакта ---\n");
                        
                char search_query[100] = {0};
                safe_read("Введите ключевое слово для поиска (0 для отмены): ", search_query, sizeof(search_query));

                if (strlen(search_query) == 0) {
                    printf("Поиск отменен: запрос пуст.\n");
                    break;
                }

                if (pb->count == 0) {
                    printf("Телефонная книга пуста, нечего искать.\n");
                    break;
                }

                Contact found_contacts[pb->count];
                int found = find_contact_with_field(pb, found_contacts, search_query);

                printf("\n=== Найдено совпадений: %d ===\n", found);
                        
                if (found == 0) {
                    printf("Контакты не найдены.\n");
                    break;
                }
                for (int i = 0; i < found; i++) {
                    printf("%d. %s %s\n", i + 1, found_contacts[i].name, found_contacts[i].surname);
                }
                        
                printf("\nВведите номер контакта для просмотра (0 для возврата в меню): ");
                int selected;
                if (scanf("%d", &selected) != 1) {
                    while (getchar() != '\n');
                    printf("Некорректный ввод.\n");
                    break;
                }
                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                if (selected == 0) break;

                if (selected < 1 || selected > found) {
                    printf("Контакта с номером %d нет в результатах поиска.\n", selected);
                    break;
                }

                int original_index = -1;
                for (int i = 0; i < pb->count; i++) {
                    if (pb->contacts[i].id == found_contacts[selected - 1].id) {
                        original_index = i;
                        break;
                    }
                }

                if (original_index == -1) {
                    printf("Ошибка: не удалось найти контакт в книге.\n");
                    break;
                }

                contact_menu(pb, original_index);
                break;
        }
            case exit_menu:
                return;
            
            default:
                printf("Нет такого пункта. Попробуйте ещё раз.\n");
            
        }
        while((c = getchar()) != '\n' && c != EOF);
    }
}