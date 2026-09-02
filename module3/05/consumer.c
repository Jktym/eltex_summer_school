#include "common.h"

int main() {
    int shmid = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shmid == -1) {
        perror("shm_open (Производитель не запущен?)");
        exit(1);
    }

    struct ShmData* shm = (struct ShmData*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem_t* sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Потребитель (PID: %d) запущен. Жду данные...\n", getpid());

    while (1) {
        sem_wait(sem);

        if (shm->free_offset == 0 && shm->producer_done) {
            sem_post(sem);
            break;
        }

        int processed_something = 0;

        if (shm->free_offset > 0) {
            size_t curr_offset = shm->head_offset;
            
            while (1) {
                struct Block* block = (struct Block*)(shm->buffer + curr_offset);
                
                if (block->count > 0) {
                    int min = block->data[0];
                    int max = block->data[0];
                    
                    for (size_t i = 1; i < block->count; i++) {
                        if (block->data[i] < min) min = block->data[i];
                        if (block->data[i] > max) max = block->data[i];
                    }
                    
                    printf("[PID: %d] Обработан блок (смещение %zu). Элементов: %zu. Min: %d, Max: %d\n", 
                           getpid(), curr_offset, block->count, min, max);

                    block->count = 0;
                    processed_something = 1;
                    break;
                }
                
                if (block->next_offset == 0) break;
                curr_offset = block->next_offset;
            }
        }

        int should_exit = (processed_something == 0 && shm->producer_done == 1);

        sem_post(sem);

        if (should_exit) {
            break;
        }

        if (processed_something) {
            usleep(250000);
        } else {
            usleep(50000);
        }
    }

    munmap(shm, SHM_SIZE);
    close(shmid);
    sem_close(sem);

    printf("Потребитель (PID: %d) завершил работу.\n", getpid());
    return 0;
}