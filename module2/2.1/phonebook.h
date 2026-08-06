#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define MAX_NAME_L 20
#define MAX_WORK_L 30
#define MAX_PHONE_L 13
#define MAX_EMAIL_L 25
#define MAX_LINK_L 100
#define PHONEBOOK_SIZE 128

#include <string.h>
#include <stdio.h>

typedef struct Contact {
    int id;
    char name [MAX_NAME_L];
    char second_name[MAX_NAME_L];
    char surname[MAX_NAME_L];
    char work[MAX_WORK_L];
    char position[MAX_WORK_L];
    char mob_phone[MAX_PHONE_L];
    char work_phone[MAX_PHONE_L];
    char email[MAX_EMAIL_L];
    char link[MAX_LINK_L];    
} Contact;

typedef struct Phonebook {
    Contact conctacts[PHONEBOOK_SIZE];
    int count;
} Phonebook;

void init_phonebook(Phonebook *pb);
int add_contact (Phonebook *pb, const char *name, const char *second_name, 
                       const char *surname, const char *work, const char *position,
                       const char *mob_phone, const char *work_phone, 
                       const char *email, const char *link);
int is_empty_string(const char *str);

#endif