#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "../common/json_output.h"
#include "shm_handler.h"

int main(int argc, char *argv[]) {
    pid_t pid;
    shm_manager_t shm_mgr;
    
    // Verifica se uma mensagem foi passada como argumento
    char *message = "Mensagem via Memória Compartilhada";
    if (argc > 1) {
        message = argv[1];
    }
    
    // Initialize shared memory (creator)
    if (init_shm(&shm_mgr, 1) == -1) {
        print_json_error("shm", "Failed to initialize shared memory", getpid());
        exit(EXIT_FAILURE);
    }
    
    char status_msg[512];
    snprintf(status_msg, sizeof(status_msg), "Criado segmento de memória compartilhada '%s' (tamanho: %d bytes)", SHM_NAME, SHM_SIZE);
    print_json_status("shm", "created", status_msg, getpid());
    
    // Fork process
    char initial_message[512];
    snprintf(initial_message, sizeof(initial_message), "Processo principal (PID: %d) recebeu a mensagem de entrada: \"%s\"", getpid(), message);
    print_json_status("shm", "received", initial_message, getpid());

    print_json_status("shm", "forking", "Criando processo filho...", getpid());
    pid = fork();
    
    if (pid == 0) {
        // Child process
        pid_t child_pid = getpid();
        // Reinitialize shared memory (non-creator)
        shm_manager_t child_shm_mgr;
        if (init_shm(&child_shm_mgr, 0) == -1) {
            print_json_error("shm", "Child failed to access shared memory", child_pid);
            exit(EXIT_FAILURE);
        }
        
        char status_msg[256];
        snprintf(status_msg, sizeof(status_msg), "Processo filho (PID: %d) aguardando o semáforo...", child_pid);
        print_json_status("shm", "sem_wait", status_msg, child_pid);
        
        // Wait for parent to post semaphore
        if (shm_sem_wait(&child_shm_mgr) == -1) {
            print_json_error("shm", "Child failed to wait for semaphore", child_pid);
            cleanup_shm(&child_shm_mgr);
            exit(EXIT_FAILURE);
        }

        snprintf(status_msg, sizeof(status_msg), "Filho (PID: %d) liberado pelo semáforo, lendo a memória...", child_pid);
        print_json_status("shm", "sem_proceed", status_msg, child_pid);
        
        // Read from shared memory
        char buffer[SHM_SIZE];
        if (read_from_shm(&child_shm_mgr, buffer, sizeof(buffer)) == 0) {
            print_json_data("shm", buffer, "child_read", getppid());
        } else {
            print_json_error("shm", "Child failed to read from shared memory", child_pid);
        }
        
        // Cleanup
        cleanup_shm(&child_shm_mgr);
        print_json_status("shm", "child_exit", "Processo filho finalizado.", child_pid);
        exit(EXIT_SUCCESS);
        
    } else if (pid > 0) {
        // Parent process
        pid_t parent_pid = getpid();
        // Write to shared memory
        char status_msg[256];
        snprintf(status_msg, sizeof(status_msg), "Processo pai (PID: %d) escrevendo na memória...", parent_pid);
        print_json_status("shm", "writing", status_msg, parent_pid);
        if (write_to_shm(&shm_mgr, message) == 0) {
            print_json_data("shm", message, "parent_write", parent_pid);
        } else {
            print_json_error("shm", "Parent failed to write to shared memory", parent_pid);
        }

        // Post semaphore to signal child
        snprintf(status_msg, sizeof(status_msg), "Pai (PID: %d) liberando o semáforo para o filho...", parent_pid);
        print_json_status("shm", "sem_post", status_msg, parent_pid);
        if (shm_sem_post(&shm_mgr) == -1) {
            print_json_error("shm", "Parent failed to post semaphore", parent_pid);
        }
        
        // Wait for child
        print_json_status("shm", "waiting_child", "Processo pai aguardando finalização do filho...", parent_pid);
        wait(NULL);
        
        // Cleanup
        cleanup_shm(&shm_mgr);
        print_json_status("shm", "completed", "Comunicação via memória compartilhada finalizada", parent_pid);
        
    } else {
        print_json_error("shm", "Failed to fork process", getpid());
        cleanup_shm(&shm_mgr);
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
