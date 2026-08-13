#include "menu.h"


int main() {
    Phonebook *pb = init_phonebook();
    menu(pb);
    return 0;
}