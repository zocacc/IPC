/**
 * @file pipe_demo.c
 * @brief Demonstração de comunicação bidirecional com pipes anônimos.
 *
 * Este programa ilustra a comunicação entre um processo pai e um filho
 * usando dois pipes anônimos: um para o pai enviar dados ao filho e outro
 * para o filho enviar dados de volta ao pai (eco).
 *
 * A saída do programa é em formato JSON para permitir a integração com
 * uma interface gráfica (frontend), com logs detalhados de cada etapa.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/json_output.h"

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_json_error("pipes", "Uso: ./pipe_demo <mensagem>", getpid());
        return 1;
    }
    const char *message_to_send = argv[1];
    char status_msg[512];

    // --- 1. SETUP ---
    print_json_status("pipes", "setup", "Iniciando a configuração dos pipes...", getpid());

    int parent_to_child_pipe[2];
    int child_to_parent_pipe[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(parent_to_child_pipe) == -1 || pipe(child_to_parent_pipe) == -1) {
        print_json_error("pipes", "Falha ao criar os pipes.", getpid());
        exit(EXIT_FAILURE);
    }
    print_json_status("pipes", "setup_complete", "Pipes de comunicação (Pai->Filho e Filho->Pai) criados.", getpid());

    // --- 2. FORKING ---
    print_json_status("pipes", "fork", "Criando processo filho...", getpid());
    pid = fork();

    if (pid == -1) {
        print_json_error("pipes", "Falha no fork().", getpid());
        exit(EXIT_FAILURE);
    }

    // --- 3. LÓGICA DO PROCESSO FILHO ---
    if (pid == 0) {
        pid_t child_pid = getpid();
        print_json_status("pipes", "child_start", "Processo filho iniciado.", child_pid);

        // Fechar pontas não utilizadas
        close(parent_to_child_pipe[1]); // Não escreve no pipe Pai->Filho
        close(child_to_parent_pipe[0]);  // Não lê no pipe Filho->Pai
        print_json_status("pipes", "child_setup", "Filho fechou pontas de pipe não utilizadas.", child_pid);

        // Ler do pai
        snprintf(status_msg, sizeof(status_msg), "Filho aguardando mensagem do pai no pipe...");
        print_json_status("pipes", "child_read_wait", status_msg, child_pid);
        ssize_t bytes_read = read(parent_to_child_pipe[0], buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            snprintf(status_msg, sizeof(status_msg), "Filho recebeu %zd bytes.", bytes_read);
            print_json_status("pipes", "child_read_ok", status_msg, child_pid);
            print_json_data("pipes", buffer, "pai -> filho", getppid());

            // Escrever eco para o pai
            snprintf(status_msg, sizeof(status_msg), "Filho enviando eco: \"%s\"", buffer);
            print_json_status("pipes", "child_write", status_msg, child_pid);
            write(child_to_parent_pipe[1], buffer, strlen(buffer) + 1);

        } else {
            print_json_error("pipes", "Filho falhou ao ler do pipe do pai.", child_pid);
        }

        // Limpeza final do filho
        close(parent_to_child_pipe[0]);
        close(child_to_parent_pipe[1]);
        print_json_status("pipes", "child_exit", "Processo filho finalizado.", child_pid);
        exit(EXIT_SUCCESS);
    }
    // --- 4. LÓGICA DO PROCESSO PAI ---
    else {
        pid_t parent_pid = getpid();
        print_json_status("pipes", "parent_start", "Pai continua execução após fork.", parent_pid);

        // Fechar pontas não utilizadas
        close(parent_to_child_pipe[0]); // Não lê no pipe Pai->Filho
        close(child_to_parent_pipe[1]);  // Não escreve no pipe Filho->Pai
        print_json_status("pipes", "parent_setup", "Pai fechou pontas de pipe não utilizadas.", parent_pid);

        // Escrever para o filho
        snprintf(status_msg, sizeof(status_msg), "Pai enviando mensagem: \"%s\"", message_to_send);
        print_json_status("pipes", "parent_write", status_msg, parent_pid);
        write(parent_to_child_pipe[1], message_to_send, strlen(message_to_send) + 1);
        print_json_data("pipes", message_to_send, "pai -> filho", parent_pid);

        // Ler eco do filho
        print_json_status("pipes", "parent_read_wait", "Pai aguardando eco do filho...", parent_pid);
        ssize_t bytes_read = read(child_to_parent_pipe[0], buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            snprintf(status_msg, sizeof(status_msg), "Pai recebeu eco de %zd bytes.", bytes_read);
            print_json_status("pipes", "parent_read_ok", status_msg, parent_pid);
            print_json_data("pipes", buffer, "filho -> pai (eco)", pid);
        } else {
            print_json_error("pipes", "Pai falhou ao ler o eco do filho.", parent_pid);
        }

        // Limpeza final do pai
        close(parent_to_child_pipe[1]);
        close(child_to_parent_pipe[0]);

        print_json_status("pipes", "parent_wait", "Pai aguardando término do processo filho...", parent_pid);
        wait(NULL);
        print_json_status("pipes", "success", "Comunicação via pipes concluída com sucesso.", parent_pid);
    }

    return 0;
}
