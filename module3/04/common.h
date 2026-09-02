#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define SHM_SIZE 4096
#define PROJ_ID 'A'
#define KEY_FILE "/tmp"

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

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
#else
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
#endif

static inline void sem_lock(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

static inline void sem_unlock(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

#endif