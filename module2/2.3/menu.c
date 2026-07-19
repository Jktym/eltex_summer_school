#include "menu.h"
#include "calc.h"


void menu() {
    const char main_menu [] = {"-------ГЛАВНОЕ МЕНЮ-------\nВыберите действие:\n1.Сложение\n2.Вычитание\n3.Умножение\n4.Деление\n0.Выход\n"};
    while (1) {
        printf("%s", main_menu);
        int choice = -1;
        scanf(" %d", &choice);
        if (choice >= 0 && choice < 5) {
            if (!choice) return;
            float x, y;
            printf("Введите первый аргумент: ");
            scanf(" %f", &x);
            printf("Введите второй аргумент: ");
            scanf(" %f", &y);
            if (choice == 4 && !y) printf("На 0 делить нельзя!!!\n");
            else {
                float result = select(choice) (x, y);
                char oper = print_oper(choice);
                printf("%.3f %c %.3f = %f\n", x, oper, y, result);
            }
        } else printf("Нет такого пункта! Попробуйте ещё раз.\n");
        int c;
        while((c = getchar()) != '\n' && c != EOF);
    }
}