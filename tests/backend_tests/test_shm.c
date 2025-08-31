/**
 * @file test_shm.c
 * @brief Teste unitário para o módulo de memória compartilhada
 * 
 * Este arquivo implementa testes automatizados para verificar o funcionamento
 * correto do sistema de memória compartilhada, incluindo criação, escrita,
 * leitura, sincronização via semáforos e limpeza de recursos.
 * 
 * @author [Seu Nome]
 * @date [Data de Criação]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shm_handler.h"
#include "json_output.h"

/**
 * @brief Executa o teste completo de memória compartilhada
 * 
 * Esta função implementa um teste end-to-end que:
 * 1. Cria um segmento de memória compartilhada no processo pai
 * 2. Cria um processo filho que se conecta ao segmento
 * 3. Coordena escrita/leitura via semáforos
 * 4. Verifica a integridade dos dados transmitidos
 * 5. Limpa todos os recursos criados
 * 
 * O teste verifica:
 * - Criação e inicialização correta do segmento
 * - Sincronização via semáforos POSIX
 * - Transmissão correta de dados entre processos
 * - Limpeza adequada de recursos
 * 
 * @note Todas as operações são logadas em formato JSON para facilitar debug
 * @note O processo filho termina com código de saída apropriado
 */
void run_test() {
    shm_manager_t shm_mgr;
    pid_t pid;

    // 1. Setup: Initialize shared memory in the parent
    if (init_shm(&shm_mgr, 1) != 0) {
        print_json_error("test_shm", "Parent failed to initialize SHM", getpid());
        return;
    }
    print_json_status("test_shm", "setup", "Parent initialized SHM successfully", getpid());

    pid = fork();

    if (pid < 0) {
        print_json_error("test_shm", "Fork failed", getpid());
        cleanup_shm(&shm_mgr); // Clean up in parent
        return;
    }

    if (pid == 0) {
        // 2. Child Process: Attach to SHM, wait for semaphore, and Read
        shm_manager_t child_shm_mgr;
        if (init_shm(&child_shm_mgr, 0) != 0) {
            print_json_error("test_shm", "Child failed to attach to SHM", getpid());
            exit(1); // Child process exits with an error code
        }

        // Wait for the parent to signal that it has written to the memory
        if (shm_sem_wait(&child_shm_mgr) != 0) {
            print_json_error("test_shm", "Child failed to wait for semaphore", getpid());
            cleanup_shm(&child_shm_mgr);
            exit(1);
        }

        char buffer[SHM_SIZE];
        if (read_from_shm(&child_shm_mgr, buffer, sizeof(buffer)) != 0) {
            print_json_error("test_shm", "Child failed to read from SHM", getpid());
            cleanup_shm(&child_shm_mgr);
            exit(1);
        }

        // 3. Child Process: Assert and report
        const char* expected_message = "Hello from parent!";
        if (strcmp(buffer, expected_message) == 0) {
            print_json_status("test_shm", "assertion_pass", "Child read the correct message", getpid());
        } else {
            char error_msg[512];
            snprintf(error_msg, sizeof(error_msg), "Child read incorrect message. Expected '%s', got '%s'", expected_message, buffer);
            print_json_error("test_shm", error_msg, getpid());
        }

        cleanup_shm(&child_shm_mgr);
        exit(0); // Child process exits successfully

    } else {
        // 4. Parent Process: Write to SHM
        const char* message = "Hello from parent!";
        if (write_to_shm(&shm_mgr, message) != 0) {
            print_json_error("test_shm", "Parent failed to write to SHM", getpid());
        } else {
            print_json_status("test_shm", "write", "Parent wrote to SHM successfully", getpid());
        }

        // Signal the child that it can now read
        if (shm_sem_post(&shm_mgr) != 0) {
            print_json_error("test_shm", "Parent failed to post semaphore", getpid());
        }

        // 5. Parent Process: Wait for child and check exit status
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            print_json_status("test_shm", "test_pass", "Shared memory test completed successfully.", getpid());
        } else {
            print_json_error("test_shm", "Test failed: Child process did not complete successfully.", getpid());
        }

        // 6. Teardown: Cleanup SHM in the parent
        cleanup_shm(&shm_mgr);
    }
}

/**
 * @brief Função principal do teste
 * 
 * Ponto de entrada para o executável de teste. Chama a função
 * de teste principal e retorna 0 para indicar sucesso.
 * 
 * @return 0 sempre (sucesso)
 */
int main() {
    run_test();
    return 0;
}
