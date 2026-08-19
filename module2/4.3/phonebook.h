#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define MAX_NAME_L 20
#define MAX_WORK_L 30
#define MAX_PHONE_L 13
#define MAX_EMAIL_L 25
#define MAX_LINK_L 100
#define PHONEBOOK_SIZE 128

#ifndef CONTACTS_FILE
#define CONTACTS_FILE "contacts.txt"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Data {
    int id;
    char name [MAX_NAME_L];
    char surname[MAX_NAME_L];
    char second_name[MAX_NAME_L];
    char work[MAX_WORK_L];
    char position[MAX_WORK_L];
    char mob_phone[MAX_PHONE_L];
    char work_phone[MAX_PHONE_L];
    char email[MAX_EMAIL_L];
    char link[MAX_LINK_L];    
} Data;

typedef struct Contact {
    Data *data;
    struct Contact *left;
    struct Contact *right;
} Contact;

typedef struct Phonebook {
    Contact *root;
    int count;
} Phonebook;

enum {
    name_field = 0,
    surname_field = 1,
    second_name_field = 2,
    work_field = 3,
    position_field = 4,
    mob_phone_field = 5,
    work_phone_field = 6,
    email_field = 7,
    link_field = 8
};

Phonebook *init_phonebook();
void save_phonebook(const Phonebook *pb);
Phonebook *add_contact(Phonebook *pb, Data new_data);
Contact input_contact();
Phonebook *delete_contact(Phonebook *pb, int choice_id);
void print_contact_info(const Contact *c);
int find_contact_with_field(const Phonebook *pb, Contact **results, const char *info);
void fill_contact_field(Data *c, char *input, int field);
void print_contact_list(Phonebook *pb);
Contact *get_contact(Phonebook *pb, int choice_id);
int get_contact_id(Contact *c);
void free_phonebook(Phonebook *pb);
void display_tree_visual(Phonebook *pb);
static void print_tree_visual_rec(Contact *root, int level);
#endif