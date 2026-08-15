#include "menu.h"

const char *fields_name[] = {"Имя: ", "Фамилия: ", "Отчество: ", "Место работы: ",
                            "Должность: ", "Моб. телефон: ", "Раб. телефон:", "Email: ", 
                            "Ссылки на соц. сети: "};
const int fields_size[] = {MAX_NAME_L, MAX_NAME_L, MAX_NAME_L, MAX_WORK_L, MAX_WORK_L,
                        MAX_PHONE_L, MAX_PHONE_L, MAX_EMAIL_L, MAX_LINK_L};

void input_contact_field(const char *prompt, char *input, int max_len) {
    printf("%s", prompt);
    if(fgets(input, max_len, stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
    } else {
        input[0] = '\0';
    }
}

void contact_menu(Phonebook *pb, int choice_id) {
    Contact *contact = get_contact(pb, choice_id);
    if (contact == NULL) {
        printf("Ошибка, контакт не найден!\n");
        return;
    }
    
    while(1) {
        print_contact_info(contact);
        printf("Выберите действие:\n1. Редактировать контакт\n2. Удалить контакт\n0. Назад\n");
        int choice;
        scanf("%d", &choice);
        switch(choice) {
            case edit_selected_contact:
                printf("Выберите поле для редактирования (0 для выхода):\n");
                for (int i = 0; i < 9; i++) {
                    printf("%d.%s\n", i+ 1, fields_name[i]);
                }
                int choice_field;
                scanf("%d", &choice_field);
                int c;
                while((c = getchar()) != '\n' && c != EOF);
                if(choice_field == 0) break;
                if (choice_field > 0 && choice_field < 10) {
                    choice_field--;
                    char new_field_input[fields_size[choice_field]];
                    input_contact_field(fields_name[choice_field], new_field_input, fields_size[choice_field]);
                    fill_contact_field(contact->data, new_field_input, choice_field);
                    } else printf("Нет такого поля.\n");
                break;
            case delete_selected_contact: {
                pb = delete_contact(pb, choice_id);
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
                    print_contact_list(pb);
                    printf("Введите номер контакта для выбора (0 для возврата в главное меню).\n");
                    int choice_id;
                    scanf("%d", &choice_id);
                    if (!choice_id) break;
                    if(choice_id < 1 || choice_id > pb->count) {
                        printf("Контакта с таким id не существует.\n");
                        break;
                    }
                    contact_menu(pb, choice_id);
                }
                break;
            case add_new_contact: {
                //int c;
                //while ((c = getchar()) != '\n' && c != EOF); // Чистим буфер
                printf("\n--- Ввод нового контакта ---\n");
                printf("(Обязательные поля помечены *)\n\n");
                
                int result = 0;
                Data new_data = {0};
                for (int i = 0; i < 9; i++) {
                    char field[fields_size[i]];
                    input_contact_field(fields_name[i], field, fields_size[i]);
                    if ((i == 0 || i == 1) && field[0] == '\0') {result = -1; break; }
                    fill_contact_field(&new_data, field, i);
                }
                if(!result) {
                    pb = add_contact(pb, new_data);
                    if (pb->head == NULL) result = -1;
                }
                
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
                input_contact_field("Введите ключевое слово для поиска (0 для отмены): ", search_query, sizeof(search_query));

                if (strlen(search_query) == 0) {
                    printf("Поиск отменен: запрос пуст.\n");
                    break;
                }

                if (pb->count == 0) {
                    printf("Телефонная книга пуста, нечего искать.\n");
                    break;
                }

                Contact *found_contacts[pb->count];
                int found = find_contact_with_field(pb, found_contacts, search_query);

                printf("\n=== Найдено совпадений: %d ===\n", found);
                        
                if (found == 0) {
                    printf("Контакты не найдены.\n");
                    break;
                }
                for (int i = 0; i < found; i++) {
                    printf("%d. %s %s\n", i + 1, found_contacts[i]->data->surname, found_contacts[i]->data->name);
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
                original_index = get_contact_id(found_contacts[selected-1]);

                if (original_index == -1) {
                    printf("Ошибка: не удалось найти контакт в книге.\n");
                    break;
                }

                contact_menu(pb, original_index);
                break;
        }
            case exit_menu:
                free_phonebook(pb);
                free(pb);
                return;
            
            default:
                printf("Нет такого пункта. Попробуйте ещё раз.\n");
            
        }
        while((c = getchar()) != '\n' && c != EOF);
    }
}