#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#define SHM_NAME "/posix_shm_task5"
#define SEM_NAME "/posix_sem_task5"
#define SHM_SIZE 4096

struct Block {
    size_t count;       
    size_t next_offset; 
    int data[];         
};

struct ShmData {
    size_t head_offset;       
    size_t last_block_offset; 
    size_t free_offset;       
    int producer_done;        
    char buffer[];            
};

#endif