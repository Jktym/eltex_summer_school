#include "common.h"
#include <poll.h>
#include <netdb.h> 

void send_file(int sock, const char *filepath) {
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        perror("Ошибка открытия файла");
        return;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);

    const char *filename = strrchr(filepath, '/');
    if (filename) filename++;
    else filename = filepath;

    int msg_type = MSG_FILE_HEADER;
    send(sock, &msg_type, sizeof(msg_type), 0);

    struct file_packet fp;
    strncpy(fp.filename, filename, sizeof(fp.filename) - 1);
    fp.file_size = file_size;
    send(sock, &fp, sizeof(fp), 0);

    msg_type = MSG_FILE_DATA;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        send(sock, &msg_type, sizeof(msg_type), 0);
        send(sock, &bytes_read, sizeof(bytes_read), 0);
        send(sock, buffer, bytes_read, 0);
    }

    close(file_fd);
    printf("[Клиент] Файл '%s' успешно отправлен.\n> ", filename);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <IP_или_Имя_сервера> <Ваше_Имя>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    char *username = argv[2];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Разрешение имени хоста 
    struct hostent *server = gethostbyname(server_ip);
    if (server == NULL) {
        fprintf(stderr, "Ошибка: невозможно найти хост '%s'\n", server_ip);
        close(sock);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Сразу отправляем имя серверу
    if (send(sock, username, strlen(username), 0) < 0) {
        perror("Ошибка отправки имени");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("=== Подключено к чату. Для отправки файла введите: /file <путь> ===\n> ");
    fflush(stdout);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sock;
    fds[1].events = POLLIN;

    char incoming_filename[256];
    long incoming_file_size = 0;
    long received_bytes_count = 0;
    FILE *incoming_file = NULL;

    while (1) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            perror("Ошибка poll");
            break;
        }

        // Чтение от сервера
        if (fds[1].revents & POLLIN) {
            int msg_type;
            ssize_t bytes = recv(sock, &msg_type, sizeof(msg_type), 0);
            if (bytes <= 0) {
                printf("\n[Сеть] Соединение с сервером разорвано.\n");
                break;
            }

            if (msg_type == MSG_TEXT) {
                char buffer[BUFFER_SIZE];
                bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    printf("\r%s\n> ", buffer);
                    fflush(stdout);
                }
            } 
            else if (msg_type == MSG_FILE_HEADER) {
                struct file_packet fp;
                recv(sock, &fp, sizeof(fp), 0);
                strncpy(incoming_filename, fp.filename, sizeof(incoming_filename));
                incoming_file_size = fp.file_size;
                received_bytes_count = 0;

                char save_path[512];
                snprintf(save_path, sizeof(save_path), "rx_%s", incoming_filename);
                incoming_file = fopen(save_path, "wb");

                printf("\n[Файл] Начался приём файла: %s (размер: %ld байт)\n> ", incoming_filename, incoming_file_size);
                fflush(stdout);
            } 
            else if (msg_type == MSG_FILE_DATA) {
                ssize_t chunk_size;
                recv(sock, &chunk_size, sizeof(chunk_size), 0);

                char buffer[BUFFER_SIZE];
                ssize_t total_read = 0;
                while (total_read < chunk_size) {
                    ssize_t r = recv(sock, buffer + total_read, chunk_size - total_read, 0);
                    if (r <= 0) break;
                    total_read += r;
                }

                if (incoming_file) {
                    fwrite(buffer, 1, total_read, incoming_file);
                    received_bytes_count += total_read;

                    if (received_bytes_count >= incoming_file_size) {
                        fclose(incoming_file);
                        incoming_file = NULL;
                        printf("\n[Файл] Файл '%s' успешно принят и сохранен как rx_%s\n> ", incoming_filename, incoming_filename);
                        fflush(stdout);
                    }
                }
            }
        }

        // Ввод с клавиатуры
        if (fds[0].revents & POLLIN) {
            char input[BUFFER_SIZE];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0;

                if (strlen(input) > 0) {
                    if (strncmp(input, "/file ", 6) == 0) {
                        send_file(sock, input + 6);
                    } else {
                        int msg_type = MSG_TEXT;
                        char full_msg[BUFFER_SIZE * 2];
                        snprintf(full_msg, sizeof(full_msg), "[%s]: %s", username, input);

                        send(sock, &msg_type, sizeof(msg_type), 0);
                        send(sock, full_msg, strlen(full_msg), 0);
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