/**
 * @file shm_handler.h
 * @brief Gerenciador de memória compartilhada com sincronização via semáforos
 * 
 * Este arquivo define estruturas e funções para gerenciar segmentos de memória
 * compartilhada entre processos, incluindo sincronização via semáforos POSIX
 * para coordenar o acesso aos dados compartilhados.
 */

#ifndef SHM_HANDLER_H
#define SHM_HANDLER_H

#include <sys/types.h>
#include <semaphore.h>

// Constantes para a memória compartilhada
#define SHM_NAME "/ipc_shm"
#define SHM_SIZE 4096

// Constantes para o semáforo
#define SEM_NAME "/ipc_sem"

/**
 * @brief Estrutura para gerenciar a memória compartilhada e a sincronização.
 * 
 * Encapsula o descritor de arquivo da memória compartilhada, um ponteiro para a
 * região de memória mapeada, um ponteiro para o semáforo de sincronização e
 * uma flag para identificar o processo criador.
 */
typedef struct {
    int shm_fd;        // Descritor de arquivo da memória compartilhada
    void *ptr;         // Ponteiro para a memória mapeada
    sem_t *sem;        // Ponteiro para o semáforo de sincronização
    int is_creator;    // Flag que indica se este processo é o criador
} shm_manager_t;

/**
 * @brief Inicializa a memória compartilhada e o semáforo.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @param create Flag: 1 para criar, 0 para apenas abrir.
 * @return 0 em sucesso, -1 em erro.
 */
int init_shm(shm_manager_t *shm_mgr, int create);

/**
 * @brief Escreve dados na memória compartilhada.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @param data A string a ser escrita.
 * @return 0 em sucesso, -1 em erro.
 */
int write_to_shm(shm_manager_t *shm_mgr, const char *data);

/**
 * @brief Lê dados da memória compartilhada.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @param buffer Buffer para armazenar os dados lidos.
 * @param size Tamanho do buffer.
 * @return 0 em sucesso, -1 em erro.
 */
int read_from_shm(shm_manager_t *shm_mgr, char *buffer, size_t size);

/**
 * @brief Libera os recursos da memória compartilhada e do semáforo.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @return 0 em sucesso, -1 em erro.
 */
int cleanup_shm(shm_manager_t *shm_mgr);

/**
 * @brief Bloqueia (espera) no semáforo.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @return 0 em sucesso, -1 em erro.
 */
int shm_sem_wait(shm_manager_t *shm_mgr);

/**
 * @brief Libera (posta) o semáforo.
 * 
 * @param shm_mgr Ponteiro para a estrutura do gerenciador.
 * @return 0 em sucesso, -1 em erro.
 */
int shm_sem_post(shm_manager_t *shm_mgr);

#endif // SHM_HANDLER_H
