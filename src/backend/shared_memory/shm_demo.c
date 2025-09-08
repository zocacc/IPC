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
    char status_msg[512];

    char *message = "Mensagem padrão via SHM";
    if (argc > 1) {
        message = argv[1];
    }
    
    // --- 1. PAI: SETUP ---
    print_json_status("shm", "setup", "Pai (Criador) iniciando configuração...", getpid());
    if (init_shm(&shm_mgr, 1) == -1) {
        print_json_error("shm", "Pai falhou ao inicializar SHM e semáforo", getpid());
        exit(EXIT_FAILURE);
    }
    snprintf(status_msg, sizeof(status_msg), "SHM ('%s') e semáforo ('%s') criados.", SHM_NAME, SEM_NAME);
    print_json_status("shm", "setup_complete", status_msg, getpid());
    
    // --- 2. PAI: FORKING ---
    print_json_status("shm", "fork", "Criando processo filho...", getpid());
    pid = fork();
    
    if (pid < 0) {
        print_json_error("shm", "Falha no fork()", getpid());
        cleanup_shm(&shm_mgr);
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // --- 3. LÓGICA DO PROCESSO FILHO ---
        pid_t child_pid = getpid();
        shm_manager_t child_shm_mgr;
        print_json_status("shm", "child_start", "Filho (Leitor) iniciado.", child_pid);

        // Anexa à SHM e ao semáforo existentes
        if (init_shm(&child_shm_mgr, 0) == -1) {
            print_json_error("shm", "Filho falhou ao se conectar à SHM", child_pid);
            exit(EXIT_FAILURE);
        }
        print_json_status("shm", "child_attach_ok", "Filho conectado aos recursos compartilhados.", child_pid);

        // Aguarda o sinal (post) do pai (bloqueante)
        print_json_status("shm", "child_sem_wait", "Filho bloqueado, aguardando sinal do pai...", child_pid);
        if (shm_sem_wait(&child_shm_mgr) == -1) {
            print_json_error("shm", "Filho falhou na espera do semáforo", child_pid);
            cleanup_shm(&child_shm_mgr);
            exit(EXIT_FAILURE);
        }

        // Lê a mensagem da memória
        print_json_status("shm", "child_read_shm", "Sinal recebido! Filho lendo da memória...", child_pid);
        char buffer[SHM_SIZE];
        if (read_from_shm(&child_shm_mgr, buffer, sizeof(buffer)) == 0) {
            print_json_data("shm", buffer, "leitura_filho", child_pid);
        } else {
            print_json_error("shm", "Filho falhou ao ler da SHM", child_pid);
        }
        
        // Limpa seus recursos e termina
        cleanup_shm(&child_shm_mgr);
        print_json_status("shm", "child_exit", "Processo filho finalizado.", child_pid);
        exit(EXIT_SUCCESS);
        
    } else {
        // --- 4. LÓGICA DO PROCESSO PAI ---
        pid_t parent_pid = getpid();
        print_json_status("shm", "parent_start", "Pai (Escritor) continua após fork.", parent_pid);

        // Escreve a mensagem na memória
        snprintf(status_msg, sizeof(status_msg), "Pai escrevendo na memória: \"%s\"", message);
        print_json_status("shm", "parent_write_shm", status_msg, parent_pid);
        if (write_to_shm(&shm_mgr, message) != 0) {
            print_json_error("shm", "Pai falhou ao escrever na SHM", parent_pid);
            shm_sem_post(&shm_mgr); // Libera o filho para não ficar preso
            wait(NULL);
            cleanup_shm(&shm_mgr);
            exit(EXIT_FAILURE);
        }
        print_json_data("shm", message, "escrita_pai", parent_pid);

        // Sinaliza para o filho que a mensagem está pronta
        print_json_status("shm", "parent_sem_post", "Pai sinalizando para o filho (sem_post)...", parent_pid);
        if (shm_sem_post(&shm_mgr) == -1) {
            print_json_error("shm", "Pai falhou ao sinalizar semáforo", parent_pid);
        }
        
        // Aguarda o término do filho
        print_json_status("shm", "parent_wait_child", "Pai aguardando finalização do filho...", parent_pid);
        wait(NULL);
        
        // Limpa os recursos (remove SHM e semáforo do sistema)
        print_json_status("shm", "parent_cleanup", "Pai limpando os recursos do sistema...", parent_pid);
        cleanup_shm(&shm_mgr);
        print_json_status("shm", "success", "Comunicação via SHM finalizada com sucesso.", parent_pid);
    }
    
    return 0;
}
