#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

enum {
    WITHOUT_PRIORITY = 0,
    WITH_PRIORITY = 1,
    NOT_LOWER_PRIORITY = 2
};

typedef struct Node {
    char message[11];
    int priority;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *head;
    Node *tail;
    int count;
} Queue;

Queue *init_queue();
Queue *add_node(Queue *q, char *message, int priority);
Node *pop(Queue *q, int have_priority, ...);
void print_queue(Queue *q);
void free_queue(Queue *q);
#endif