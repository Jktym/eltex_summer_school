#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>

#define KEY 0x1234
#define MAX_MSG 1024

struct msg {
    long type;
    char text[MAX_MSG];
};

int qid = -1;
int run = 1;

struct topic_node {
    char name[64];
    struct topic_node *next;
};

struct sub_node {
    int pid;
    struct topic_node *topics;
    struct sub_node *next;
};

struct pub_node {
    int pid;
    struct pub_node *next;
};

void handle_sigint(int sig) {
    run = 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Использование: %s -b | -p <тема> | -s <тема1>...\n", argv[0]);
        return 1;
    }

    signal(SIGINT, handle_sigint);
    
    char role = argv[1][1];

    switch (role) {
        case 'b': {
            qid = msgget(KEY, IPC_CREAT | IPC_EXCL | 0666);
            if (qid < 0) {
                perror("Брокер уже запущен или произошла ошибка");
                return 1;
            }
            
            struct sub_node *subs = NULL;
            struct pub_node *pubs = NULL;
            struct msg m;
            
            printf("Брокер запущен. Ожидание сообщений...\n");

            while (run && msgrcv(qid, &m, MAX_MSG, 1, 0) > 0) {
                int pid;
                char cmd[16];
                char topic[64];
                
                if (sscanf(m.text, "%15[^,],%d,%63[^,\n]", cmd, &pid, topic) == 3) {
                    
                    if (strcmp(cmd, "subscribe") == 0) {
                        struct sub_node *s = subs;
                        while (s != NULL && s->pid != pid) {
                            s = s->next;
                        }
                        
                        if (s == NULL) {
                            s = malloc(sizeof(struct sub_node));
                            s->pid = pid;
                            s->topics = NULL;
                            s->next = subs;
                            subs = s;
                        }
                        
                        struct topic_node *t = s->topics;
                        while (t != NULL && strcmp(t->name, topic) != 0) {
                            t = t->next;
                        }
                        
                        if (t == NULL) {
                            t = malloc(sizeof(struct topic_node));
                            strcpy(t->name, topic);
                            t->next = s->topics;
                            s->topics = t;
                        }

                    } else if (strcmp(cmd, "unsubscribe") == 0) {
                        struct sub_node **s_ptr = &subs;
                        while (*s_ptr != NULL) {
                            if ((*s_ptr)->pid == pid) {
                                struct topic_node **t_ptr = &((*s_ptr)->topics);
                                while (*t_ptr != NULL) {
                                    if (strcmp((*t_ptr)->name, topic) == 0) {
                                        struct topic_node *del_t = *t_ptr;
                                        *t_ptr = del_t->next;
                                        free(del_t);
                                        break;
                                    }
                                    t_ptr = &((*t_ptr)->next);
                                }
                                
                                if ((*s_ptr)->topics == NULL) {
                                    struct sub_node *del_s = *s_ptr;
                                    *s_ptr = del_s->next;
                                    free(del_s);
                                    continue;
                                }
                            }
                            s_ptr = &((*s_ptr)->next);
                        }

                    } else if (strcmp(cmd, "send") == 0) {
                        struct pub_node *p = pubs;
                        while (p != NULL && p->pid != pid) {
                            p = p->next;
                        }
                        if (p == NULL) {
                            p = malloc(sizeof(struct pub_node));
                            p->pid = pid;
                            p->next = pubs;
                            pubs = p;
                        }
                        
                        char *payload = strchr(m.text, '\n');
                        if (payload != NULL) {
                            payload++;
                            
                            struct sub_node *s = subs;
                            while (s != NULL) {
                                struct topic_node *t = s->topics;
                                while (t != NULL) {
                                    if (strcmp(t->name, topic) == 0) {
                                        struct msg out;
                                        out.type = s->pid;
                                        strncpy(out.text, payload, MAX_MSG);
                                        msgsnd(qid, &out, strlen(out.text) + 1, 0);
                                        break;
                                    }
                                    t = t->next;
                                }
                                s = s->next;
                            }
                        }
                    }
                }
            }
            
            printf("\nБрокер завершает работу. Рассылка сигналов и очистка памяти...\n");
            
            while (subs != NULL) {
                kill(subs->pid, SIGINT);
                
                struct topic_node *t = subs->topics;
                while (t != NULL) {
                    struct topic_node *del_t = t;
                    t = t->next;
                    free(del_t);
                }
                
                struct sub_node *del_s = subs;
                subs = subs->next;
                free(del_s);
            }
            
            while (pubs != NULL) {
                kill(pubs->pid, SIGINT);
                
                struct pub_node *del_p = pubs;
                pubs = pubs->next;
                free(del_p);
            }
            
            msgctl(qid, IPC_RMID, NULL);
            printf("Очередь сообщений удалена.\n");
            
            break;
        }
            
        case 'p': {
            if (argc < 3) {
                printf("Укажите тему для публикации. Пример: -p sport\n");
                return 1;
            }
            
            qid = msgget(KEY, 0666);
            if (qid < 0) {
                perror("Брокер не найден (очередь не существует)");
                return 1;
            }
            
            struct msg m;
            m.type = 1;
            
            char payload[512];
            char topic[64];
            strcpy(topic, argv[2]);
            int pid = getpid();
            
            printf("Издатель готов (тема: %s, PID: %d). Введите сообщения:\n", topic, pid);
            
            while (run && fgets(payload, sizeof(payload), stdin) != NULL) {
                snprintf(m.text, MAX_MSG, "send,%d,%s\n%s", pid, topic, payload);
                
                if (msgsnd(qid, &m, strlen(m.text) + 1, 0) < 0) {
                    break;
                }
            }
            
            printf("\nИздатель завершил работу.\n");
            break;
        }

        case 's': {
            if (argc < 3) {
                printf("Укажите хотя бы одну тему. Пример: -s sport news\n");
                return 1;
            }
            
            qid = msgget(KEY, 0666);
            if (qid < 0) {
                perror("Брокер не найден (очередь не существует)");
                return 1;
            }
            
            int pid = getpid();
            struct msg m;
            m.type = 1;
            
            for (int i = 2; i < argc; i++) {
                snprintf(m.text, MAX_MSG, "subscribe,%d,%s", pid, argv[i]);
                msgsnd(qid, &m, strlen(m.text) + 1, 0);
            }
            
            printf("Подписчик %d ожидает сообщений...\n", pid);
            
            while (run && msgrcv(qid, &m, MAX_MSG, pid, 0) > 0) {
                printf("[Получено] %s", m.text);
            }
            
            for (int i = 2; i < argc; i++) {
                m.type = 1; 
                snprintf(m.text, MAX_MSG, "unsubscribe,%d,%s", pid, argv[i]);
                msgsnd(qid, &m, strlen(m.text) + 1, IPC_NOWAIT);
            }
            
            printf("\nПодписчик завершил работу.\n");
            break;
        }
            
        default:
            printf("Неверная роль. Используйте -b, -p или -s\n");
            return 1;
    }

    return 0;
}