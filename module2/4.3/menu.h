#ifndef MENU_H
#define MENU_H

#include "phonebook.h"

enum main_menu_items {
    print_contacts = 1,
    add_new_contact = 2,
    find_contact = 3,
    print_tree = 4,
    exit_menu = 0
};

enum contact_menu_items {
    edit_selected_contact = 1,
    delete_selected_contact = 2,
    back = 0
};

void contact_menu(Phonebook *pb, int choice_id);
void menu(Phonebook *pb);

#endif