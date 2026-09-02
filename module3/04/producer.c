#include "common.h"

int main() {
    srand(time(NULL));

    key_t key = ftok(KEY_FILE, PROJ_ID);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    struct ShmData* shm = (struct ShmData*)shmat(shmid, NULL, 0);

    int semid = semget(key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    shm->head_offset = 0;
    shm->last_block_offset = 0;
    shm->free_offset = 0;
    shm->producer_done = 0;

    size_t max_buffer_size = SHM_SIZE - sizeof(struct ShmData);
    printf("Производитель запущен. Идет генерация данных...\n");

    while (1) {
        sem_lock(semid);

        int num_elements = rand() % 15 + 5;
        size_t block_size = sizeof(struct Block) + num_elements * sizeof(int);

        if (shm->free_offset + block_size > max_buffer_size) {
            shm->producer_done = 1;
            sem_unlock(semid);
            printf("\nПамять исчерпана. Остановка генерации. Ждем потребителей...\n");
            break;
        }

        struct Block* new_block = (struct Block*)(shm->buffer + shm->free_offset);
        new_block->count = num_elements;
        new_block->next_offset = 0;

        for (int i = 0; i < num_elements; i++) {
            new_block->data[i] = rand() % 1000;
        }

        if (shm->free_offset == 0) {
            shm->head_offset = shm->free_offset;
        } else {
            struct Block* last_block = (struct Block*)(shm->buffer + shm->last_block_offset);
            last_block->next_offset = shm->free_offset;
        }

        printf("Создан блок на %d эл-тов (смещение: %zu)\n", num_elements, shm->free_offset);

        shm->last_block_offset = shm->free_offset;
        shm->free_offset += block_size;

        sem_unlock(semid);
        usleep(100000);
    }

    while (1) {
        sem_lock(semid);
        int all_processed = 1;

        if (shm->free_offset > 0) {
            size_t curr_offset = shm->head_offset;
            while (1) {
                struct Block* block = (struct Block*)(shm->buffer + curr_offset);
                if (block->count > 0) {
                    all_processed = 0;
                    break;
                }
                if (block->next_offset == 0) break;
                curr_offset = block->next_offset;
            }
        }

        sem_unlock(semid);

        if (all_processed) {
            printf("\nВсе блоки обработаны! Завершаю работу.\n");
            break;
        }
        
        usleep(500000);
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    
    printf("Ресурсы освобождены.\n");
    return 0;
}