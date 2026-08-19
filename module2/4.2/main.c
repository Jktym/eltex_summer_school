#include "menu.h"

int main() {
    srand(time(NULL));
    Queue *q = init_queue();
    menu(q);
    return 0;
}