#include <stdio.h>
#include "p2p_chat.c"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Укажите название очереди.\n");
        return 1;
    }
    char *q_name = argv[1];

    char q1_name[100];
    strcpy(q1_name, q_name);
    strcat(q1_name, "_1");

    char q2_name[100];
    strcpy(q2_name, q_name);
    strcat(q2_name, "_2");

    
    return 0;
}