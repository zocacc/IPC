#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <errno.h>

#include "socket_demo.h"
#include "../common/json_output.h"

#define BUFFER_SIZE 256

// Protótipos das funções
void run_server(const char* initial_message);
void run_client(const char* message);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <mensagem>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        print_json_error("socket_demo", "Falha no fork()", getpid());
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo Filho (Cliente)
        // Adiciona um pequeno atraso para garantir que o servidor esteja pronto
        sleep(1);
        run_client(argv[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Processo Pai (Servidor)
        run_server(argv[1]);
        // Espera o processo filho terminar
        wait(NULL);
    }

    return 0;
}

void run_server(const char* initial_message) {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Criar o socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        print_json_error("socket_server", "Falha ao criar socket", getpid());
        return;
    }

    // Garante que o arquivo de socket não exista antes de fazer o bind
    unlink(SOCKET_PATH);

    // 2. Configurar o endereço do servidor
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Associar o socket ao endereço (bind)
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        print_json_error("socket_server", "Falha no bind", getpid());
        close(server_fd);
        return;
    }

    // 4. Marcar o socket para escutar conexões
    if (listen(server_fd, 5) == -1) {
        print_json_error("socket_server", "Falha no listen", getpid());
        close(server_fd);
        return;
    }

    print_json_status("socket_server", "listening", "Servidor aguardando conexão...", getpid());

    // 5. Aceitar a conexão do cliente
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        print_json_error("socket_server", "Falha no accept", getpid());
        close(server_fd);
        return;
    }
    print_json_status("socket_server", "accepted", "Cliente conectado.", getpid());

    // 6. Receber dados do cliente
    ssize_t num_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        print_json_data("socket_server", buffer, "cliente -> servidor", getpid());

        // 7. Enviar uma resposta (eco)
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Eco do servidor: %s", buffer);
        if (send(client_fd, response, strlen(response), 0) == -1) {
            print_json_error("socket_server", "Falha ao enviar resposta", getpid());
        }
    } else {
        print_json_error("socket_server", "Falha ao receber dados do cliente", getpid());
    }

    // 8. Fechar os descritores e limpar
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    print_json_status("socket_server", "shutdown", "Servidor finalizado.", getpid());
}

void run_client(const char* message) {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Criar o socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        print_json_error("socket_client", "Falha ao criar socket", getpid());
        return;
    }

    // 2. Configurar o endereço do servidor
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Conectar ao servidor
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        print_json_error("socket_client", "Falha ao conectar", getpid());
        close(client_fd);
        return;
    }
    print_json_status("socket_client", "connected", "Conectado ao servidor.", getpid());

    // 4. Enviar a mensagem inicial
    if (send(client_fd, message, strlen(message), 0) == -1) {
        print_json_error("socket_client", "Falha ao enviar mensagem", getpid());
        close(client_fd);
        return;
    }
    print_json_data("socket_client", message, "cliente -> servidor (enviando)", getpid());

    // 5. Receber a resposta do servidor
    ssize_t num_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        print_json_data("socket_client", buffer, "servidor -> cliente (eco)", getpid());
    } else {
        print_json_error("socket_client", "Falha ao receber resposta", getpid());
    }

    // 6. Fechar o socket
    close(client_fd);
    print_json_status("socket_client", "finished", "Cliente finalizado.", getpid());
}