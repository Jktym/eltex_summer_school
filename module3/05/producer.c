#include "common.h"

int main() {
    srand(time(NULL));

    int shmid = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shmid == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shmid, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    struct ShmData* shm = (struct ShmData*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem_t* sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    shm->head_offset = 0;
    shm->last_block_offset = 0;
    shm->free_offset = 0;
    shm->producer_done = 0;

    size_t max_buffer_size = SHM_SIZE - sizeof(struct ShmData);
    printf("Производитель запущен. Идет генерация данных...\n");

    while (1) {
        sem_wait(sem);

        int num_elements = rand() % 15 + 5;
        size_t block_size = sizeof(struct Block) + num_elements * sizeof(int);

        if (shm->free_offset + block_size > max_buffer_size) {
            shm->producer_done = 1;
            sem_post(sem);
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

        sem_post(sem);
        usleep(100000);
    }

    while (1) {
        sem_wait(sem);
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

        sem_post(sem);

        if (all_processed) {
            printf("\nВсе блоки обработаны! Завершаю работу.\n");
            break;
        }
        
        usleep(500000);
    }

    munmap(shm, SHM_SIZE);
    close(shmid);
    shm_unlink(SHM_NAME);

    sem_close(sem);
    sem_unlink(SEM_NAME);

    printf("Ресурсы освобождены.\n");
    return 0;
}