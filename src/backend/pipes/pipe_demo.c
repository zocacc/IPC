/**
 * @file pipe_demo.c
 * @brief Demonstração de comunicação bidirecional com pipes anônimos.
 *
 * Este programa ilustra a comunicação entre um processo pai e um filho
 * usando dois pipes anônimos: um para o pai enviar dados ao filho e outro
 * para o filho enviar dados de volta ao pai (eco).
 *
 * A saída do programa é em formato JSON para permitir a integração com
 * uma interface gráfica (frontend).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/json_output.h" // Funções para imprimir em formato JSON

#define BUFFER_SIZE 256 // Define um tamanho para o buffer de mensagens

int main(int argc, char *argv[]) {
    // 1. Validação dos Argumentos de Entrada
    // O programa espera uma mensagem como argumento para ser enviada via pipe.
    if (argc < 2) {
        print_json_error("pipes", "Erro: Uma mensagem é necessária. Uso: ./pipe_demo <sua-mensagem>", 0);
        return 1;
    }
    const char *message_to_send = argv[1];

    // 2. Declaração dos Pipes e Variáveis
    int parent_to_child_pipe[2]; // Pipe 1: Pai -> Filho
    int child_to_parent_pipe[2]; // Pipe 2: Filho -> Pai
    pid_t pid;                   // Para armazenar o ID do processo filho
    char buffer[BUFFER_SIZE];    // Buffer para ler as mensagens recebidas

    print_json_status("pipes", "info", "Iniciando a demonstração de pipes.", getpid());

    // 3. Criação dos Pipes
    // A função pipe() cria um par de descritores de arquivo:
    // - descritor[0] é para leitura
    // - descritor[1] é para escrita
    if (pipe(parent_to_child_pipe) == -1 || pipe(child_to_parent_pipe) == -1) {
        print_json_error("pipes", "Falha ao criar os pipes.", getpid());
        exit(EXIT_FAILURE);
    }
    print_json_status("pipes", "info", "Pipes criados com sucesso.", getpid());

    // 4. Criação do Processo Filho (Fork)
    // O fork() cria uma cópia do processo atual.
    // - No processo pai, fork() retorna o PID do filho.
    // - No processo filho, fork() retorna 0.
    // - Em caso de erro, retorna -1.
    pid = fork();

    if (pid == -1) {
        print_json_error("pipes", "Falha ao criar o processo filho (fork).", getpid());
        exit(EXIT_FAILURE);
    }

    // 5. Lógica do Processo Filho
    if (pid == 0) {
        pid_t child_pid = getpid();
        pid_t parent_pid = getppid(); // PID do processo pai
        print_json_status("pipes", "info", "Processo filho iniciado.", child_pid);

        // 5.1. Fechar as pontas não utilizadas dos pipes no filho
        // O filho vai ler do pipe "pai->filho", então fecha a ponta de escrita.
        close(parent_to_child_pipe[1]);
        // O filho vai escrever no pipe "filho->pai", então fecha a ponta de leitura.
        close(child_to_parent_pipe[0]);

        // 5.2. Ler a mensagem enviada pelo pai
        ssize_t bytes_read = read(parent_to_child_pipe[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Garante que a string termina com null
            // Ao logar a mensagem recebida, usa o PID do pai (remetente)
            print_json_data("pipes", buffer, "pai -> filho", parent_pid);

            // 5.3. Enviar a mesma mensagem de volta para o pai (eco)
            print_json_status("pipes", "info", "Filho ecoando a mensagem para o pai.", child_pid);
            write(child_to_parent_pipe[1], buffer, strlen(buffer) + 1);
        } else {
            print_json_error("pipes", "Filho não conseguiu ler a mensagem do pai.", child_pid);
        }

        // 5.4. Fechar as pontas restantes e terminar
        close(parent_to_child_pipe[0]);
        close(child_to_parent_pipe[1]);
        print_json_status("pipes", "info", "Processo filho finalizado.", child_pid);
        exit(EXIT_SUCCESS);
    }
    // 6. Lógica do Processo Pai
    else {
        pid_t parent_pid = getpid();
        print_json_status("pipes", "info", "Processo pai continua a execução.", parent_pid);

        // 6.1. Fechar as pontas não utilizadas dos pipes no pai
        // O pai vai escrever no pipe "pai->filho", então fecha a ponta de leitura.
        close(parent_to_child_pipe[0]);
        // O pai vai ler do pipe "filho->pai", então fecha a ponta de escrita.
        close(child_to_parent_pipe[1]);

        // 6.2. Enviar a mensagem para o filho
        print_json_status("pipes", "info", "Pai enviando mensagem para o filho.", parent_pid);
        write(parent_to_child_pipe[1], message_to_send, strlen(message_to_send) + 1);
        print_json_data("pipes", message_to_send, "pai -> filho", parent_pid);

        // 6.3. Ler o eco do filho
        ssize_t bytes_read = read(child_to_parent_pipe[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            // Ao logar o eco recebido, usa o PID do filho (remetente)
            print_json_data("pipes", buffer, "filho -> pai (eco)", pid);
        } else {
            print_json_error("pipes", "Pai não conseguiu ler o eco do filho.", parent_pid);
        }

        // 6.4. Fechar as pontas restantes
        close(parent_to_child_pipe[1]);
        close(child_to_parent_pipe[0]);

        // 6.5. Esperar o processo filho terminar para evitar zumbis
        wait(NULL);
        print_json_status("pipes", "success", "Demonstração de pipes concluída.", parent_pid);
    }

    return 0;
}