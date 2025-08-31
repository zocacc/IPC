#ifndef SHM_HANDLER_H
#define SHM_HANDLER_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

#define SHM_NAME "/ipc_shm"
#define SEM_NAME "/ipc_sem"
#define SHM_SIZE 4096

typedef struct {
    int shm_fd;
    void *ptr;
    sem_t *sem;
    int is_creator;
} shm_manager_t;

// Function declarations
int init_shm(shm_manager_t *shm_mgr, int create);
int write_to_shm(shm_manager_t *shm_mgr, const char *data);
int read_from_shm(shm_manager_t *shm_mgr, char *buffer, size_t size);
int cleanup_shm(shm_manager_t *shm_mgr);
int shm_sem_wait(shm_manager_t *shm_mgr);
int shm_sem_post(shm_manager_t *shm_mgr);

#endif // SHM_HANDLER_H