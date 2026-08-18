#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "menu.h"

int main() {
    void *handle_add, *handle_sub, *handle_mul, *handle_div;
    operation_func add, sub, mul, div_func; 

    handle_add = dlopen("./dinamyc_libs/libadd.so", RTLD_LAZY);
    handle_sub = dlopen("./dinamyc_libs/libsub.so", RTLD_LAZY);
    handle_mul = dlopen("./dinamyc_libs/libmul.so", RTLD_LAZY);
    handle_div = dlopen("./dinamyc_libs/libdiv.so", RTLD_LAZY);

    if (!handle_add || !handle_div || !handle_mul || !handle_sub) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        exit(1);
    }

    add = (operation_func) dlsym(handle_add, "add");
    sub = (operation_func) dlsym(handle_sub, "sub");
    mul = (operation_func) dlsym(handle_mul, "mul");
    div_func = (operation_func) dlsym(handle_div, "div");

    if(!add || !sub || !mul || !div_func) {
        fprintf(stderr, "Ошибка получения функции: %s\n", dlerror());
        dlclose(handle_add);
        dlclose(handle_sub);
        dlclose(handle_mul);
        dlclose(handle_div);
        exit(1);
    }

    operation operations[] = {
        {"Сложение", add},
        {"Вычитание", sub},
        {"Умножение", mul},
        {"Деление", div_func}
    };

    menu(operations);

    dlclose(handle_add);
    dlclose(handle_sub);
    dlclose(handle_mul);
    dlclose(handle_div);

    return 0;
}