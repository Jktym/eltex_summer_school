#include "menu.h"


int main() {
    Phonebook pb;
    init_phonebook(&pb);
    menu(&pb);
    return 0;
}