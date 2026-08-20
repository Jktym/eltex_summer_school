#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#define MAX_MSG_SIZE 256
#define PRIO_NORMAL 0
#define PRIO_STOP   10

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_очереди>\n", argv[0]);
        fprintf(stderr, "Пример: %s /mychat\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char base_name[128];
    if (argv[1][0] != '/') {
        snprintf(base_name, sizeof(base_name), "/%s", argv[1]);
    } else {
        strncpy(base_name, argv[1], sizeof(base_name));
    }

    char q1_name[140], q2_name[140];
    snprintf(q1_name, sizeof(q1_name), "%s_1", base_name);
    snprintf(q2_name, sizeof(q2_name), "%s_2", base_name);

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_MSG_SIZE,
        .mq_curmsgs = 0
    };

    mqd_t q_recv, q_send;
    int is_creator = 0;

    q_recv = mq_open(q1_name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    
    if (q_recv != (mqd_t)-1) {
        is_creator = 1;
        
        q_send = mq_open(q2_name, O_WRONLY | O_CREAT | O_EXCL, 0666, &attr);
        if (q_send == (mqd_t)-1) {
            perror("Ошибка создания второй очереди");
            exit(EXIT_FAILURE);
        }
        printf("[+] Очереди созданы. Ожидание собеседника...\n");
    } else if (errno == EEXIST) {
        is_creator = 0;
        
        q_send = mq_open(q1_name, O_WRONLY);
        q_recv = mq_open(q2_name, O_RDONLY);
        
        if (q_send == (mqd_t)-1 || q_recv == (mqd_t)-1) {
            perror("Ошибка подключения к очередям");
            exit(EXIT_FAILURE);
        }
        printf("[+] Подключение успешно. Можно писать сообщения.\n");
    } else {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = q_recv;
    fds[1].events = POLLIN;

    printf("Чат запущен (Ctrl+C для выхода)\n> ");
    fflush(stdout);

    char buffer[MAX_MSG_SIZE];

    while (keep_running) {
        int ret = poll(fds, 2, -1); 
        
        if (ret < 0) {
            if (errno == EINTR) continue; 
            perror("poll error");
            break;
        }

        if (fds[1].revents & POLLIN) {
            unsigned int prio;
            ssize_t bytes_read = mq_receive(q_recv, buffer, MAX_MSG_SIZE, &prio);
            if (bytes_read >= 0) {
                if (prio == PRIO_STOP) { 
                    printf("\n[Система]: Собеседник отключился.\n");
                    break; 
                }
                printf("\r[Собеседник]: %s\n> ", buffer);
                fflush(stdout);
            }
        }

        if (fds[0].revents & POLLIN) {
            if (fgets(buffer, MAX_MSG_SIZE, stdin) != NULL) {
                buffer[strcspn(buffer, "\n")] = '\0';
                
                if (mq_send(q_send, buffer, strlen(buffer) + 1, PRIO_NORMAL) < 0) {
                    perror("Ошибка отправки");
                }
                printf("> ");
                fflush(stdout);
            } else {
                break;
            }
        }
    }

    printf("\nЗавершение работы сеанса...\n");

    char stop_msg[] = "quit";
    mq_send(q_send, stop_msg, sizeof(stop_msg), PRIO_STOP);

    mq_close(q_recv);
    mq_close(q_send);

    if (is_creator) {
        mq_unlink(q1_name);
        mq_unlink(q2_name);
        printf("Очереди %s и %s удалены.\n", q1_name, q2_name);
    }

    return 0;
}