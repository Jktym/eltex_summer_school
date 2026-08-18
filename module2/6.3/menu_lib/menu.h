#ifndef MENU_H
#define MENU_H

#include <stdio.h>

typedef float (*operation_func) (float, float);
typedef struct {
    const char *name;
    operation_func func;
} operation;

void menu(operation *operations);

#endif