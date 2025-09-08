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

void run_server();
void run_client(const char* message);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_json_error("socket", "Uso: ./socket_demo <mensagem>", getpid());
        return 1;
    }

    // Garante que o arquivo de socket de uma execução anterior seja removido
    unlink(SOCKET_PATH);
    
    print_json_status("socket", "fork", "Criando processo filho (cliente)...", getpid());
    pid_t pid = fork();

    if (pid < 0) {
        print_json_error("socket", "Falha no fork()", getpid());
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo Filho (Cliente)
        run_client(argv[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Processo Pai (Servidor)
        run_server();
        print_json_status("socket", "parent_wait", "Servidor aguardando término do cliente...", getpid());
        wait(NULL);
        print_json_status("socket", "shutdown", "Comunicação via socket finalizada.", getpid());
    }

    return 0;
}

void run_server() {
    pid_t pid = getpid();
    char status_msg[512];
    
    print_json_status("socket_server", "init", "Servidor (Pai) iniciado.", pid);

    // 1. Criar o socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        print_json_error("socket_server", "Falha ao criar socket", pid);
        return;
    }
    print_json_status("socket_server", "socket_ok", "Socket do servidor criado.", pid);

    // 2. Configurar o endereço e fazer o bind
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        print_json_error("socket_server", "Falha no bind", pid);
        close(server_fd);
        return;
    }
    snprintf(status_msg, sizeof(status_msg), "Socket associado ao caminho: %s", SOCKET_PATH);
    print_json_status("socket_server", "bind_ok", status_msg, pid);

    // 3. Escutar por conexões
    if (listen(server_fd, 5) == -1) {
        print_json_error("socket_server", "Falha no listen", pid);
        close(server_fd);
        return;
    }
    print_json_status("socket_server", "listening", "Servidor escutando por conexões...", pid);

    // 4. Aceitar a conexão do cliente (bloqueante)
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        print_json_error("socket_server", "Falha no accept", pid);
        close(server_fd);
        return;
    }
    print_json_status("socket_server", "accepted", "Conexão do cliente aceita.", pid);

    // 5. Receber dados do cliente (bloqueante)
    char buffer[BUFFER_SIZE];
    print_json_status("socket_server", "recv_wait", "Servidor aguardando mensagem do cliente...", pid);
    ssize_t num_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        snprintf(status_msg, sizeof(status_msg), "Servidor recebeu %zd bytes.", num_bytes);
        print_json_status("socket_server", "recv_ok", status_msg, pid);
        print_json_data("socket_server", buffer, "cliente -> servidor", pid);

        // 6. Enviar uma resposta (eco)
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Eco do servidor: %s", buffer);
        print_json_status("socket_server", "send_echo", "Servidor enviando eco para o cliente...", pid);
        send(client_fd, response, strlen(response), 0);

    } else {
        print_json_error("socket_server", "Falha ao receber dados do cliente", pid);
    }

    // 7. Fechar os descritores e limpar
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    print_json_status("socket_server", "closed", "Recursos do servidor liberados.", pid);
}

void run_client(const char* message) {
    pid_t pid = getpid();
    char status_msg[512];

    print_json_status("socket_client", "init", "Cliente (Filho) iniciado.", pid);

    // 1. Criar o socket
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        print_json_error("socket_client", "Falha ao criar socket", pid);
        return;
    }
    print_json_status("socket_client", "socket_ok", "Socket do cliente criado.", pid);

    // 2. Configurar o endereço do servidor
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Adiciona um pequeno atraso para dar tempo ao servidor de iniciar.
    // Em um cenário real, usaríamos um loop de retentativa.
    usleep(100000); // 100ms

    // 3. Conectar ao servidor (bloqueante)
    snprintf(status_msg, sizeof(status_msg), "Cliente tentando conectar a %s...", SOCKET_PATH);
    print_json_status("socket_client", "connecting", status_msg, pid);
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        print_json_error("socket_client", "Falha ao conectar ao servidor", pid);
        close(client_fd);
        return;
    }
    print_json_status("socket_client", "connected", "Conectado ao servidor.", pid);

    // 4. Enviar a mensagem inicial
    print_json_status("socket_client", "sending", "Cliente enviando mensagem...", pid);
    ssize_t bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent > 0) {
        print_json_data("socket_client", message, "cliente -> servidor", pid);
    } else {
        print_json_error("socket_client", "Falha ao enviar mensagem", pid);
    }


    // 5. Receber a resposta do servidor (bloqueante)
    char buffer[BUFFER_SIZE];
    print_json_status("socket_client", "recv_wait", "Cliente aguardando eco do servidor...", pid);
    ssize_t num_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        snprintf(status_msg, sizeof(status_msg), "Cliente recebeu %zd bytes.", num_bytes);
        print_json_status("socket_client", "recv_ok", status_msg, pid);
        print_json_data("socket_client", buffer, "servidor -> cliente (eco)", pid);
    } else {
        print_json_error("socket_client", "Falha ao receber resposta do servidor", pid);
    }

    // 6. Fechar o socket
    close(client_fd);
    print_json_status("socket_client", "closed", "Cliente finalizado.", pid);
}
