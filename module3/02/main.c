#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>

#define KEY 0x1234
#define MAX_MSG 1024

struct msg { long type; char text[MAX_MSG]; };
int qid = -1, run = 1;

// Структуры для динамических списков
struct topic_node { char name[64]; struct topic_node *next; };
struct sub_node   { int pid; struct topic_node *topics; struct sub_node *next; };
struct pub_node   { int pid; struct pub_node *next; };

void handle_sigint(int sig) { run = 0; }

int main(int argc, char **argv) {
    if (argc < 2) return printf("Usage: %s -b | -p <topic> | -s <topic1>...\n", argv[0]), 1;
    signal(SIGINT, handle_sigint);
    
    switch (argv[1][1]) {
        case 'b': {
            qid = msgget(KEY, IPC_CREAT | IPC_EXCL | 0666);
            if (qid < 0) return perror("Broker already exists or error"), 1;
            
            struct sub_node *subs = NULL;
            struct pub_node *pubs = NULL;
            struct msg m;
            printf("Broker started.\n");

            while (run && msgrcv(qid, &m, MAX_MSG, 1, 0) > 0) {
                int pid; char cmd[16], topic[64];
                if (sscanf(m.text, "%15[^,],%d,%63[^,\n]", cmd, &pid, topic) == 3) {
                    if (!strcmp(cmd, "subscribe")) {
                        struct sub_node *s = subs;
                        while (s && s->pid != pid) s = s->next;
                        if (!s) { 
                            s = malloc(sizeof(*s)); s->pid = pid; s->topics = NULL;
                            s->next = subs; subs = s;
                        }
                        struct topic_node *t = s->topics;
                        while (t && strcmp(t->name, topic)) t = t->next;
                        if (!t) { 
                            t = malloc(sizeof(*t)); strcpy(t->name, topic);
                            t->next = s->topics; s->topics = t;
                        }
                    } else if (!strcmp(cmd, "unsubscribe")) {
                        for (struct sub_node **s = &subs; *s; ) {
                            if ((*s)->pid == pid) {
                                for (struct topic_node **t = &(*s)->topics; *t; ) {
                                    if (!strcmp((*t)->name, topic)) {
                                        struct topic_node *del = *t; *t = del->next; free(del); break;
                                    }
                                    t = &(*t)->next;
                                }
                                if (!(*s)->topics) { 
                                    struct sub_node *del = *s; *s = del->next; free(del); continue;
                                }
                            }
                            s = &(*s)->next;
                        }
                    } else if (!strcmp(cmd, "send")) {
                        struct pub_node *p = pubs;
                        while (p && p->pid != pid) p = p->next;
                        if (!p) { 
                            p = malloc(sizeof(*p)); p->pid = pid; p->next = pubs; pubs = p;
                        }
                        char *payload = strchr(m.text, '\n');
                        if (payload++) {
                            for (struct sub_node *s = subs; s; s = s->next) {
                                for (struct topic_node *t = s->topics; t; t = t->next) {
                                    if (!strcmp(t->name, topic)) {
                                        struct msg out = { s->pid };
                                        strncpy(out.text, payload, MAX_MSG);
                                        msgsnd(qid, &out, strlen(out.text) + 1, 0);
                                        break; 
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Очистка памяти и рассылка сигналов завершения
            while (subs) {
                kill(subs->pid, SIGINT);
                struct topic_node *t = subs->topics;
                while (t) { struct topic_node *del = t; t = t->next; free(del); }
                struct sub_node *del = subs; subs = subs->next; free(del);
            }
            while (pubs) {
                kill(pubs->pid, SIGINT);
                struct pub_node *del = pubs; pubs = pubs->next; free(del);
            }
            msgctl(qid, IPC_RMID, NULL);
            printf("\nBroker shutdown.\n");
            break;
        }
            
        case 'p': {
            if (argc < 3) return printf("Provide topic.\n"), 1;
            if ((qid = msgget(KEY, 0666)) < 0) return perror("No broker"), 1;
            
            struct msg m = {1}; char payload[512];
            printf("Publisher ready (topic: %s). Enter messages:\n", argv[2]);
            while (run && fgets(payload, sizeof(payload), stdin)) {
                snprintf(m.text, MAX_MSG, "send,%d,%s\n%s", getpid(), argv[2], payload);
                if (msgsnd(qid, &m, strlen(m.text) + 1, 0) < 0) break;
            }
            break;
        }

        case 's': {
            if (argc < 3) return printf("Provide at least 1 topic.\n"), 1;
            if ((qid = msgget(KEY, 0666)) < 0) return perror("No broker"), 1;
            
            int pid = getpid(); struct msg m = {1};
            for (int i = 2; i < argc; i++) {
                snprintf(m.text, MAX_MSG, "subscribe,%d,%s", pid, argv[i]);
                msgsnd(qid, &m, strlen(m.text) + 1, 0);
            }
            
            printf("Subscriber %d listening...\n", pid);
            while (run && msgrcv(qid, &m, MAX_MSG, pid, 0) > 0) {
                printf("[Recv] %s", m.text);
            }
            
            for (int i = 2; i < argc; i++) {
                m.type = 1; snprintf(m.text, MAX_MSG, "unsubscribe,%d,%s", pid, argv[i]);
                msgsnd(qid, &m, strlen(m.text) + 1, IPC_NOWAIT);
            }
            printf("\nSubscriber shutdown.\n");
            break;
        }
            
        default:
            printf("Invalid role. Use -b, -p or -s\n");
            return 1;
    }
    return 0;
}
