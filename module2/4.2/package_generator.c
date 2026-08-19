#include "package_generator.h"

static char gen_random_char() {

    int letter_reg = rand() % 2;
    int letter_shift = rand() % 26; 
    
    char random_letter;
    if (letter_reg) random_letter = 'a' + letter_shift;
   else random_letter = 'A' + letter_shift;

   return random_letter;
}

void gen_package(char *buffer) {
    for (int i = 0; i < 10; i++) {
        buffer[i] = gen_random_char();
    }
    buffer[10] = '\0';
}

void add_package_to_q(Queue *q, int count) {
    char message[11];
    // if (priority == -1) {
    //     priority = rand() % 255;
    // }
    int priority = 0;
    for (int i = 0; i < count; i++) {
        priority = rand() % 255;
        gen_package(message);
        q = add_node(q, message, priority);
    }
}