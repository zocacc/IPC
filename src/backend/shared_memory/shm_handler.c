#include "shm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

int init_shm(shm_manager_t *shm_mgr, int create) {
    if (create) {
        // Create or truncate shared memory object
        shm_mgr->shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_mgr->shm_fd == -1) {
            perror("shm_open create");
            return -1;
        }
        
        // Set size
        if (ftruncate(shm_mgr->shm_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            shm_unlink(SHM_NAME); // Clean up on failure
            return -1;
        }
        
        // Create semaphore, initialize to 0
        shm_mgr->sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
        if (shm_mgr->sem == SEM_FAILED) {
            perror("sem_open create");
            shm_unlink(SHM_NAME); // Clean up shm
            return -1;
        }
        
        shm_mgr->is_creator = 1;
    } else {
        // Open existing shared memory object
        shm_mgr->shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
        if (shm_mgr->shm_fd == -1) {
            perror("shm_open open");
            return -1;
        }
        
        // Open existing semaphore
        shm_mgr->sem = sem_open(SEM_NAME, 0);
        if (shm_mgr->sem == SEM_FAILED) {
            perror("sem_open open");
            return -1;
        }
        shm_mgr->is_creator = 0;
    }
    
    // Map memory
    shm_mgr->ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_mgr->shm_fd, 0);
    if (shm_mgr->ptr == MAP_FAILED) {
        perror("mmap");
        if (create) {
            sem_close(shm_mgr->sem);
            sem_unlink(SEM_NAME);
            shm_unlink(SHM_NAME);
        }
        return -1;
    }
    
    return 0;
}

int write_to_shm(shm_manager_t *shm_mgr, const char *data) {
    if (!shm_mgr || !data) {
        return -1;
    }
    
    size_t len = strlen(data);
    if (len >= SHM_SIZE) {
        fprintf(stderr, "Data too large for shared memory\n");
        return -1;
    }
    
    memcpy(shm_mgr->ptr, data, len + 1); // Include null terminator
    return 0;
}

int read_from_shm(shm_manager_t *shm_mgr, char *buffer, size_t size) {
    if (!shm_mgr || !buffer || size == 0) {
        return -1;
    }
    
    strncpy(buffer, (char*)shm_mgr->ptr, size - 1);
    buffer[size - 1] = '\0'; // Ensure null termination
    return 0;
}

int cleanup_shm(shm_manager_t *shm_mgr) {
    if (!shm_mgr) {
        return -1;
    }
    
    // Unmap memory
    if (munmap(shm_mgr->ptr, SHM_SIZE) == -1) {
        perror("munmap");
        // Continue cleanup even if munmap fails
    }
    
    // Close file descriptor
    if (close(shm_mgr->shm_fd) == -1) {
        perror("close");
    }
    
    // Close semaphore
    if (sem_close(shm_mgr->sem) == -1) {
        perror("sem_close");
    }
    
    // Unlink shared memory and semaphore if creator
    if (shm_mgr->is_creator) {
        if (shm_unlink(SHM_NAME) == -1) {
            perror("shm_unlink");
        }
        if (sem_unlink(SEM_NAME) == -1) {
            perror("sem_unlink");
        }
    }
    
    return 0;
}

int shm_sem_wait(shm_manager_t *shm_mgr) {
    if (!shm_mgr || !shm_mgr->sem) return -1;
    return sem_wait(shm_mgr->sem);
}

int shm_sem_post(shm_manager_t *shm_mgr) {
    if (!shm_mgr || !shm_mgr->sem) return -1;
    return sem_post(shm_mgr->sem);
}
