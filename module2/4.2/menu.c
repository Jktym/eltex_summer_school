#include "menu.h"

void menu(Queue *q) {
    
    while(1) {
        printf("=======ГЛАВНОЕ МЕНЮ=======\nВыберите действие:\n1.Отобразить очередь\n2.Добавить элемент в очередь\n3.Добавить несколько элементов в очередь\n4.Извлечь первый элемент в очереди\n5.Извлечь элемент в очереди с заданным приоритетом\n6.Извлечь элемент из очереди с приоритетом не ниже заданного\n0.Выход\n");
        int choice = -1;
        scanf("%d", &choice);
        int priority = -1;
        switch (choice) {
            case 1:
                print_queue(q);
                break;
            case 2:
                add_package_to_q(q, -1);
                break;
            case 3:
                int count = 0;
                printf("Введите количество элементов: ");
                scanf("%d", &count);
                if (count >= 0 && count <= 100) {
                    add_package_to_q(q, count);
                } else printf("Недопустимое значение.\n");
                break;
            case 4:
                Node *popped = pop(q, WITHOUT_PRIORITY);
                if (popped) {
                    printf("Извлечён элемент: <Сообщение: %s | Приоритет: %d\n>", popped->message, popped->priority);
                    free(popped);
                } else printf("Очередь пуста.\n");
                break;
            case 5:
                priority = -1;
                printf("Введите приоритет: ");
                scanf("%d", &priority);
                if (priority >= 0 && priority <= 255) {
                    Node *popped = pop(q, WITH_PRIORITY, priority);
                    if (popped) {
                        printf("Извлечён элемент: <Сообщение: %s | Приоритет: %d\n>", popped->message, popped->priority);
                        free(popped);
                    } else printf("Очередь пуста.\n");
                } else printf("Недопустимое значение.\n");
                break;
            case 6:
                priority = -1;
                printf("Введите приоритет: ");
                scanf("%d", &priority);
                if (priority >= 0 && priority <= 255) {
                    Node *popped = pop(q, NOT_LOWER_PRIORITY, priority);
                    if (popped) {
                        printf("Извлечён элемент: <Сообщение: %s | Приоритет: %d\n>", popped->message, popped->priority);
                        free(popped);
                    } else printf("Очередь пуста.\n");
                } else printf("Недопустимое значение.\n");
                break;
            case 0:
                free_queue(q);
                return;
            default:
                printf("Нет такого пункта.\n");
        int c;
        while((c = getchar()) != '\n' && c != EOF);
        }
    }
}