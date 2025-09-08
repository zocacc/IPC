#include "shm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Função para limpar todos os recursos em caso de falha na inicialização
static void init_cleanup_on_failure(shm_manager_t *shm_mgr) {
    if (shm_mgr->shm_fd != -1) {
        close(shm_mgr->shm_fd);
    }
    shm_unlink(SHM_NAME);
    if (shm_mgr->sem != SEM_FAILED) {
        sem_close(shm_mgr->sem);
    }
    sem_unlink(SEM_NAME);
}

int init_shm(shm_manager_t *shm_mgr, int create) {
    shm_mgr->shm_fd = -1;
    shm_mgr->sem = SEM_FAILED;
    shm_mgr->is_creator = create;

    if (create) {
        // Garante que não haja lixo de execuções anteriores
        shm_unlink(SHM_NAME);
        sem_unlink(SEM_NAME);

        // Criar o objeto de memória compartilhada
        shm_mgr->shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_mgr->shm_fd == -1) {
            perror("shm_open");
            return -1;
        }

        // Definir o tamanho da memória compartilhada
        if (ftruncate(shm_mgr->shm_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            init_cleanup_on_failure(shm_mgr);
            return -1;
        }

        // Criar o semáforo, inicializado em 0 (bloqueado)
        shm_mgr->sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
        if (shm_mgr->sem == SEM_FAILED) {
            perror("sem_open");
            init_cleanup_on_failure(shm_mgr);
            return -1;
        }
    } else {
        // Abrir um objeto de memória compartilhada existente
        shm_mgr->shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
        if (shm_mgr->shm_fd == -1) {
            perror("shm_open (non-creator)");
            return -1;
        }

        // Abrir um semáforo existente
        shm_mgr->sem = sem_open(SEM_NAME, 0);
        if (shm_mgr->sem == SEM_FAILED) {
            perror("sem_open (non-creator)");
            close(shm_mgr->shm_fd);
            return -1;
        }
    }

    // Mapear a memória compartilhada no espaço de endereçamento do processo
    shm_mgr->ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_mgr->shm_fd, 0);
    if (shm_mgr->ptr == MAP_FAILED) {
        perror("mmap");
        if (create) {
            init_cleanup_on_failure(shm_mgr);
        } else {
            close(shm_mgr->shm_fd);
            sem_close(shm_mgr->sem);
        }
        return -1;
    }

    return 0;
}

int write_to_shm(shm_manager_t *shm_mgr, const char *data) {
    if (strlen(data) + 1 > SHM_SIZE) {
        fprintf(stderr, "Error: Data is too large for the shared memory segment.\n");
        return -1;
    }
    // Usar strcpy para copiar a string, incluindo o terminador nulo
    strcpy((char *)shm_mgr->ptr, data);
    return 0;
}

int read_from_shm(shm_manager_t *shm_mgr, char *buffer, size_t size) {
    if (size == 0) return -1;
    // Usar strncpy para evitar overflow do buffer
    strncpy(buffer, (char *)shm_mgr->ptr, size - 1);
    buffer[size - 1] = '\0'; // Garantir terminação nula
    return 0;
}

int cleanup_shm(shm_manager_t *shm_mgr) {
    // Desmapear a memória
    if (munmap(shm_mgr->ptr, SHM_SIZE) == -1) {
        perror("munmap");
    }

    // Fechar o descritor de arquivo
    if (close(shm_mgr->shm_fd) == -1) {
        perror("close");
    }

    // Fechar o semáforo
    if (sem_close(shm_mgr->sem) == -1) {
        perror("sem_close");
    }

    // Se for o criador, remover os objetos do sistema
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
    return sem_wait(shm_mgr->sem);
}

int shm_sem_post(shm_manager_t *shm_mgr) {
    return sem_post(shm_mgr->sem);
}