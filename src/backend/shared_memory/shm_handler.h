/**
 * @file shm_handler.h
 * @brief Gerenciador de memória compartilhada com sincronização via semáforos
 * 
 * Este arquivo define estruturas e funções para gerenciar segmentos de memória
 * compartilhada entre processos, incluindo sincronização via semáforos POSIX
 * para coordenar o acesso aos dados compartilhados.
 * 
 * @author [Seu Nome]
 * @date [Data de Criação]
 */

#ifndef SHM_HANDLER_H
#define SHM_HANDLER_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

/**
 * @brief Nome do segmento de memória compartilhada
 * 
 * Este nome é usado para criar/abrir o segmento de memória compartilhada
 * no sistema de arquivos /dev/shm (Linux) ou similar.
 */
#define SHM_NAME "/ipc_shm"

/**
 * @brief Nome do semáforo para sincronização
 * 
 * Nome do semáforo POSIX usado para coordenar o acesso à memória
 * compartilhada entre processos pai e filho.
 */
#define SEM_NAME "/ipc_sem"

/**
 * @brief Tamanho do segmento de memória compartilhada em bytes
 * 
 * Define o tamanho máximo de dados que podem ser armazenados
 * no segmento de memória compartilhada.
 */
#define SHM_SIZE 4096

/**
 * @brief Estrutura para gerenciar memória compartilhada
 * 
 * Esta estrutura encapsula todos os recursos necessários para
 * trabalhar com memória compartilhada: descritor de arquivo,
 * ponteiro para a memória mapeada, semáforo e flag de criação.
 */
typedef struct {
    int shm_fd;        /**< Descritor de arquivo do segmento de memória */
    void *ptr;         /**< Ponteiro para a memória mapeada */
    sem_t *sem;        /**< Ponteiro para o semáforo de sincronização */
    int is_creator;    /**< Flag indicando se este processo criou o segmento */
} shm_manager_t;

/**
 * @brief Inicializa o gerenciador de memória compartilhada
 * 
 * Cria ou abre um segmento de memória compartilhada e inicializa
 * o semáforo associado. O processo criador (is_creator=1) é responsável
 * por limpar os recursos ao final.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @param create Flag indicando se deve criar (1) ou apenas abrir (0) o segmento
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note Se create=1, o segmento e semáforo são criados do zero
 * @note Se create=0, o processo tenta abrir um segmento existente
 * @note Em caso de erro, a estrutura shm_mgr não é modificada
 */
int init_shm(shm_manager_t *shm_mgr, int create);

/**
 * @brief Escreve dados na memória compartilhada
 * 
 * Copia os dados fornecidos para o segmento de memória compartilhada.
 * A função assume que há espaço suficiente no segmento.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @param data String contendo os dados a serem escritos
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note Os dados são copiados como string terminada em null
 * @note Não há verificação de tamanho - assume-se que cabe em SHM_SIZE
 */
int write_to_shm(shm_manager_t *shm_mgr, const char *data);

/**
 * @brief Lê dados da memória compartilhada
 * 
 * Copia os dados do segmento de memória compartilhada para o buffer
 * fornecido. A função assume que o buffer tem tamanho suficiente.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @param buffer Buffer de destino para os dados lidos
 * @param size Tamanho do buffer de destino
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note Os dados são lidos como string terminada em null
 * @note O buffer deve ter pelo menos size bytes disponíveis
 */
int read_from_shm(shm_manager_t *shm_mgr, char *buffer, size_t size);

/**
 * @brief Limpa recursos de memória compartilhada
 * 
 * Desmapeia a memória, fecha o descritor de arquivo e, se for o
 * processo criador, remove o segmento e semáforo do sistema.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note Apenas o processo criador (is_creator=1) remove recursos do sistema
 * @note Outros processos apenas desmapeiam e fecham descritores
 */
int cleanup_shm(shm_manager_t *shm_mgr);

/**
 * @brief Aguarda o semáforo (operação de decremento)
 * 
 * Bloqueia até que o semáforo possa ser decrementado (valor > 0).
 * Usado para sincronizar o acesso à memória compartilhada.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note A função bloqueia indefinidamente até que o semáforo seja liberado
 * @note Útil para coordenar leitura/escrita entre processos
 */
int shm_sem_wait(shm_manager_t *shm_mgr);

/**
 * @brief Libera o semáforo (operação de incremento)
 * 
 * Incrementa o valor do semáforo, potencialmente liberando
 * outros processos que estejam aguardando.
 * 
 * @param shm_mgr Ponteiro para a estrutura de gerenciamento
 * @return 0 em caso de sucesso, -1 em caso de erro
 * 
 * @note Usado para sinalizar que a memória compartilhada está disponível
 * @note Típico padrão: processo pai escreve → libera semáforo → processo filho lê
 */
int shm_sem_post(shm_manager_t *shm_mgr);

#endif // SHM_HANDLER_H