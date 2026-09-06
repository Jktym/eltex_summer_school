#include "common.h"
#include <sys/epoll.h>

#define MAX_EVENTS 64

struct ClientInfo {
    int fd;
    char name[64];
};

struct ClientInfo clients[MAX_CLIENTS];
int client_count = 0;

void add_client(int fd, const char *name) {
    if (client_count < MAX_CLIENTS) {
        clients[client_count].fd = fd;
        strncpy(clients[client_count].name, name, sizeof(clients[client_count].name) - 1);
        client_count++;
    }
}

void remove_client(int fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].fd == fd) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
}

// Рассылка данных всем клиентам, кроме отправителя
void broadcast(int sender_fd, const void *data, size_t len) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].fd != sender_fd) {
            send(clients[i].fd, data, len, MSG_NOSIGNAL);
        }
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1 error");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl server_fd error");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];
    printf("Сервер запущен на порту %d...\n", PORT);

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                // Новое подключение
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd < 0) {
                    perror("accept error");
                    continue;
                }

                char name[64];
                int bytes = recv(client_fd, name, sizeof(name) - 1, 0);
                if (bytes > 0) {
                    name[bytes] = '\0';
                    name[strcspn(name, "\r\n")] = 0;
                } else {
                    strcpy(name, "Anonymous");
                }

                add_client(client_fd, name);

                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
                    perror("epoll_ctl client_fd error");
                    close(client_fd);
                    remove_client(client_fd);
                    continue;
                }

                printf("[Сервер] Подключился участник: %s\n", name);
                
                // Оповещаем остальных участников
                char join_msg[128];
                int msg_type = MSG_TEXT;
                snprintf(join_msg, sizeof(join_msg), "[Сеть] Появился новый участник: %s", name);
                
                for (int j = 0; j < client_count; j++) {
                    if (clients[j].fd != client_fd) {
                        send(clients[j].fd, &msg_type, sizeof(msg_type), MSG_NOSIGNAL);
                        send(clients[j].fd, join_msg, strlen(join_msg), MSG_NOSIGNAL);
                    }
                }

            } else {
                // Данные от существующего клиента
                int client_fd = events[i].data.fd;
                
                char client_name[64] = "Unknown";
                for (int j = 0; j < client_count; j++) {
                    if (clients[j].fd == client_fd) {
                        strcpy(client_name, clients[j].name);
                        break;
                    }
                }

                char buffer[BUFFER_SIZE];
                ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytes_received <= 0) {
                    // Клиент отключился
                    printf("[Сервер] Участник %s отключился.\n", client_name);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);
                    remove_client(client_fd);

                    char leave_msg[128];
                    int msg_type = MSG_TEXT;
                    snprintf(leave_msg, sizeof(leave_msg), "[Сеть] Участник %s вышел из сети.", client_name);
                    broadcast(-1, &msg_type, sizeof(msg_type));
                    broadcast(-1, leave_msg, strlen(leave_msg));
                } else {
                    // Ретранслируем полученные данные остальным
                    broadcast(client_fd, buffer, bytes_received);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}