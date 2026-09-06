#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 7777
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

// Типы сообщений для протокола
#define MSG_TEXT 1
#define MSG_FILE_HEADER 2
#define MSG_FILE_DATA 3

// Структура для передачи заголовка файла
struct file_packet {
    char filename[256];
    long file_size;
};

#endif