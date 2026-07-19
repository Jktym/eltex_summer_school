#include "calc.h"

float add(float x, float y) {
    return (x + y);
}
float sub(float x, float y) {
    return (x - y);
}
float mul(float x, float y) {
    return (x * y);
}
float div(float x, float y) {
    return (x / y);
}

char print_oper(int choice) {
    switch(choice) {
        case 1: return '+';
        case 2: return '-';
        case 3: return '*';
        case 4: return '/';
    }
}

float (*select (int choice)) (float, float) {
    switch(choice) {
        case 1: return add;
        case 2: return sub;
        case 3: return mul;
        case 4: return div;
    }
}