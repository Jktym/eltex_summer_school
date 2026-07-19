#include <stdio.h>
#include "calc.h"

void menu() {
    const char* main_menu [] = {"-------ГЛАВНОЕ МЕНЮ-------\nВыберите действие:\n1.Сложение\n2.Вычитание\n3.Умножение\n4.Деление\n0.Выход\n"};
    while(1) {
        printf("%s", *main_menu);
        int choice = -1;
        scanf("%d", &choice);
        int x, y;
        if(choice && choice >= 1 && choice <= 4) {
            printf("Введите первый аргумент: ");
            scanf(" %d", &x);
            printf("Введите второй аргумент: ");
            scanf(" %d", &y);
        }
        switch (choice) {
            case 1:
                printf("%d + %d = %d\n", x, y, add(x, y));
                break;
            case 2:
                printf("%d - %d = %d\n", x, y, sub(x, y));
                break;
            case 3:
                printf("%d * %d = %d\n", x, y, mul(x, y));
                break;
            case 4:
                if (y)
                printf("%d / %d = %d\n", x, y, div(x, y));
                else printf("На 0 делить нельзя!!\n");
                break;
            case 0:
                return;
            default:
                printf("Нет такого пункта! Попробуйте ещё раз\n");
                int c;
                while((c = getchar()) != '\n' && c != EOF);
                break;
        }
    }

}

int main() {
    menu();
    return 0;
}