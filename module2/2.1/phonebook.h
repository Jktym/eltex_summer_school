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

typedef struct Contact {
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
} Contact;

typedef struct Phonebook {
    Contact contacts[PHONEBOOK_SIZE];
    int count;
} Phonebook;

void init_phonebook(Phonebook *pb);
void save_phonebook(const Phonebook *pb);
int add_contact(Phonebook *pb, const Contact *new_contact);
int is_empty_string(const char *str);
void safe_read(const char *prompt, char *input, int max_len);
Contact input_contact();
int find_contact_with_id(Phonebook *pb, int selected_id);
int delete_contact (Phonebook *pb, int selected_id);
void print_contact_info(const Contact *c);
void edit_contact(Phonebook *pb, int current_id, int field);
int find_contact_with_field(const Phonebook *pb, Contact *results, const char *info);
#endif