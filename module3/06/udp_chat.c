#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <signal.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int sock = -1;

void handle_sigint(int sig) {
    if (sock != -1) {
        char *offline_msg = "OFFLINE";
        struct sockaddr_in broadcast_addr;
        
        memset(&broadcast_addr, 0, sizeof(broadcast_addr));
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_port = htons(PORT);
        broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

        sendto(sock, offline_msg, strlen(offline_msg), 0,
               (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
        
        close(sock);
    }
    printf("\nВыход из чата.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <Ваше_Имя>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *username = argv[1];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    int broadcast_perm = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_perm, sizeof(broadcast_perm)) < 0) {
        perror("Ошибка настройки SO_BROADCAST");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Ошибка настройки SO_REUSEADDR");
        close(sock);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Ошибка привязки сокета (bind)");
        close(sock);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    char join_msg[BUFFER_SIZE];
    snprintf(join_msg, sizeof(join_msg), "JOIN:%s", username);
    if (sendto(sock, join_msg, strlen(join_msg), 0,
               (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("Ошибка отправки приветственного сообщения");
    }

    printf("=== Чат запущен. Для выхода нажмите Ctrl+C ===\n");

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sock;         
    fds[1].events = POLLIN;

    while (1) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            perror("Ошибка poll");
            break;
        }

        if (fds[1].revents & POLLIN) {
            char buffer[BUFFER_SIZE];
            struct sockaddr_in sender_addr;
            socklen_t addr_len = sizeof(sender_addr);

            ssize_t bytes_received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                              (struct sockaddr *)&sender_addr, &addr_len);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';

                if (strncmp(buffer, "JOIN:", 5) == 0) {
                    printf("\n[Сеть] Появился новый участник: %s\n> ", buffer + 5);
                    fflush(stdout);
                } else if (strcmp(buffer, "OFFLINE") == 0) {
                    printf("\n[Сеть] Один из участников вышел из сети.\n> ");
                    fflush(stdout);
                } else {
                    printf("\r%s\n> ", buffer);
                    fflush(stdout);
                }
            }
        }

        if (fds[0].revents & POLLIN) {
            char input[BUFFER_SIZE];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0;

                if (strlen(input) > 0) {
                    char full_msg[BUFFER_SIZE* 2];
                    snprintf(full_msg, sizeof(full_msg), "[%s]: %s", username, input);

                    if (sendto(sock, full_msg, strlen(full_msg), 0,
                               (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
                        perror("Ошибка отправки сообщения");
                    }
                }
                printf("> ");
                fflush(stdout);
            }
        }
    }

    close(sock);
    return 0;
}