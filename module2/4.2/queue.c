#include "queue.h"

Queue *init_queue() {
    Queue *queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;

    return queue;
}

Queue *add_node(Queue *q, char *message, int priority) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return q;

    new_node->next = NULL;
    strcpy(new_node->message, message);
    new_node->priority = priority;
    


    if (q->head == NULL) {
        q->head = new_node;
        q->tail = new_node;
    } else {
        q->tail->next = new_node;
        q->tail = new_node;
    }
    q->count++;
    return q;
    
} 

Node *pop(Queue *q, int have_priority, ...) {
    if (!q || !q->head) return NULL;
    
    Node *current = q->head;
    Node *prev = NULL; 
    va_list spec_priority;
    
    switch(have_priority) {
        case WITHOUT_PRIORITY:
            break; 
            
        case WITH_PRIORITY: {
            va_start(spec_priority, have_priority);
            int priority = va_arg(spec_priority, int);
            va_end(spec_priority);
            
            if (priority < 0 || priority > 255) return NULL;
            
            while (current != NULL) {
                if (current->priority == priority) break;
                prev = current;
                current = current->next;
            }
            break;
        }
        case NOT_LOWER_PRIORITY: {
            va_start(spec_priority, have_priority);
            int not_lower_priority = va_arg(spec_priority, int);
            va_end(spec_priority);
            
            if(not_lower_priority < 0 || not_lower_priority > 255) return NULL;
            
            while (current != NULL) {
                if(current->priority >= not_lower_priority) break;
                prev = current;
                current = current->next;
            }
            break;
        }
        default:
            return NULL;
    }
    
    if (current != NULL) { 
        
        if (prev == NULL) {
            q->head = current->next;
        } else {
            prev->next = current->next;
        }
        
        if (current == q->tail) {
            q->tail = prev;
        }
        
        current->next = NULL; 
        
        q->count--;
    }

    return current;
}


void print_queue(Queue *q) {
    if (!q) return;
    Node *current = q->head;
    int i = 0;
    printf("В очереди всего: %d\n", q->count);
    while (current) {
        printf("<Сообщение: %s | Приоритет: %d> <---  ", current->message, current->priority);
        current = current->next;
        i++;
        if(i % 5 == 0) printf("\n");
    }
        printf("\n\nНажмите Enter, чтобы выйти в главное меню...");
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); 
}

void free_queue(Queue *q) {
    if(!q) return;
    Node *current = q->head;
    Node *target_for_free;
    while(current) {
        target_for_free = current;
        current = current->next;
        free(target_for_free);
    }
}