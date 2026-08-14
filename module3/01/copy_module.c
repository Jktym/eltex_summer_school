#include "copy_module.h"

int copy_file(char *filepath, int use_named_pipe, const char *pipe_name) {
    int pipe_fd[2];

    if (use_named_pipe) {
        unlink(pipe_name); 
        if (mkfifo(pipe_name, 0666) == -1) {
            perror("Ошибка при создании FIFO");
            return 1;
        }
    } else {
        if (pipe(pipe_fd) == -1) {
            perror("Ошибка при создании pipe");
            return 1;
        }
    }

    pid_t pid = fork();

    if(pid == -1) {
        perror("Ошибка fork");
        if (use_named_pipe) unlink(pipe_name);
        return 1;
        
    } else if(pid == 0) {
        int read_fd;
        
        if (use_named_pipe) {
            read_fd = open(pipe_name, O_RDONLY);
            if (read_fd == -1) {
                perror("Ошибка открытия FIFO для чтения");
                exit(1);
            }
        } else {
            close(pipe_fd[1]);
            read_fd = pipe_fd[0]; 
        }

        char copy_filepath[100];
        strcpy(copy_filepath, filepath);
        strcat(copy_filepath, ".copy");
        
        int dest_fd = open(copy_filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) {
            fprintf(stderr, "Ошибка при открытии '%s': %s\n", copy_filepath, strerror(errno));
            exit(1);
        }

        char buffer[CHUNK_SIZE];
        ssize_t bytes_read;

        while((bytes_read = read(read_fd, buffer, CHUNK_SIZE)) > 0) {
            ssize_t total_written = 0;
            while(total_written < bytes_read) {
                ssize_t written = write(dest_fd, buffer + total_written, bytes_read - total_written);
                if(written == -1) {
                    perror("Ошибка записи в файл-копию");
                    break;
                }
                total_written += written;
            }
        }

        close(dest_fd);
        close(read_fd);
        exit(0);

    } else {
        int write_fd;
        
        if (use_named_pipe) {
            write_fd = open(pipe_name, O_WRONLY);
            if (write_fd == -1) {
                perror("Ошибка открытия FIFO для записи");
                unlink(pipe_name);
                return 1;
            }
        } else {
            close(pipe_fd[0]);
            write_fd = pipe_fd[1]; 
        }

        int src_fd = open(filepath, O_RDONLY);
        if (src_fd == -1) {
            fprintf(stderr, "Ошибка при открытии файла '%s': %s\n", filepath, strerror(errno));  
            close(write_fd);
            wait(NULL);
            if (use_named_pipe) unlink(pipe_name);
            return 1;
        } else {
            printf("Файл '%s' открыт.\n", filepath);
        }
    
        char buffer[CHUNK_SIZE];
        ssize_t bytes_read;
        
        while((bytes_read = read(src_fd, buffer, CHUNK_SIZE)) > 0) {
            ssize_t total_written = 0;
            while(total_written < bytes_read) {
                ssize_t written = write(write_fd, buffer + total_written, bytes_read - total_written);
                if(written == -1) {
                    perror("Ошибка записи в канал");
                    break;
                }
                total_written += written;
            }
        }

        close(src_fd);
        close(write_fd);
        wait(NULL);
        
        if (use_named_pipe) {
            unlink(pipe_name); 
        }
    }
    return 0;
}