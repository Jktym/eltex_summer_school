#include <stdio.h>
#include <math.h>
#include "menu.h"

void menu(operation *operations) {
    while (1) {
        printf("-------ГЛАВНОЕ МЕНЮ-------\nВыберите действие:\n");
        for (int i = 0; i < 4; i++) {
            printf("%d. %s\n", i + 1, operations[i].name); 
        }
        
        printf("0. Выход\nВаш выбор: ");
        int choice = -1;
        scanf(" %d", &choice);    
        
        if (choice >= 1 && choice <= 4) {
            float x, y;
            printf("Введите первый аргумент: ");
            scanf(" %f", &x);
            printf("Введите второй аргумент: ");
            scanf(" %f", &y);
            
            float result = operations[choice - 1].func(x, y);
            
            if (isinf(result) || isnan(result)) {
                printf("Ошибка вычисления (возможно, деление на 0)!\n\n");
                continue;
            }
            printf("Результат: %.3f\n\n", result);
        } else if (choice == 0) {
            return;
        } else {
            printf("Нет такого пункта! Попробуйте ещё раз.\n\n");
        }
        
        int c;
        while((c = getchar()) != '\n' && c != EOF);
    }
}