#ifndef COPY_MODULE_H
#define COPY_MODULE_H


#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h> 

#define CHUNK_SIZE 4096

// Добавили const char *pipe_name
int copy_file(char *filepath, int use_named_pipe, const char *pipe_name); 

#endif