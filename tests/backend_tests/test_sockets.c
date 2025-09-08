#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "json_output.h"

#define BUFFER_SIZE 4096

// Função para executar um comando e capturar sua saída
char* execute_command(const char* command) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        return NULL;
    }

    char* output = (char*)malloc(BUFFER_SIZE);
    if (!output) {
        pclose(pipe);
        return NULL;
    }
    output[0] = '\0';

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        strncat(output, buffer, BUFFER_SIZE - strlen(output) - 1);
    }

    pclose(pipe);
    return output;
}

void run_socket_test() {
    const char* executable = "./socket_demo";
    const char* test_message = "hello_from_socket_ctest";
    char command[512];
    snprintf(command, sizeof(command), "%s %s", executable, test_message);

    // O executável socket_demo ainda não existe, então o teste vai falhar.
    // Precisamos compilar primeiro.
    print_json_status("socket_test", "setup", "Compilando o projeto com build.sh...", getpid());
    system("./build.sh > /dev/null 2>&1");


    char* output = execute_command(command);

    if (!output) {
        print_json_error("socket_test", "Falha ao executar socket_demo. Verifique se foi compilado.", getpid());
        return;
    }

    // Filtra a saída para exibir apenas os JSONs de dados, de forma mais robusta.
    printf("--- JSON de Comunicação de Dados (Socket Test) ---\n");
    char* output_copy = strdup(output);
    if (output_copy) {
        char* line = output_copy;
        while (1) {
            char* next_line = strchr(line, '\n');
            if (next_line) *next_line = '\0';

            if (strstr(line, "\"type\":\"data\"")) {
                printf("%s\n", line);
            }

            if (next_line) {
                line = next_line + 1;
            } else {
                break;
            }
        }
        free(output_copy);
    }
    printf("--- Fim do JSON de Comunicação ---\n\n");

    // Verificações do fluxo de comunicação
    int status_ok = 1;
    if (strstr(output, "\"module\":\"socket_server\"") == NULL ||
        strstr(output, "\"status\":\"listening\"") == NULL) {
        status_ok = 0;
    }
    if (strstr(output, "\"module\":\"socket_client\"") == NULL ||
        strstr(output, "\"status\":\"connected\"") == NULL) {
        status_ok = 0;
    }
    if (strstr(output, test_message) == NULL) {
        status_ok = 0;
    }
    if (strstr(output, "Eco do servidor:") == NULL) {
        status_ok = 0;
    }

    if (status_ok) {
        print_json_status("socket_test", "test_pass", "Teste de Sockets concluído com sucesso.", getpid());
    } else {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Teste de Sockets falhou. Saída completa:\n%s", output);
        print_json_error("socket_test", error_msg, getpid());
    }

    free(output);
}

int main() {
    run_socket_test();
    return 0;
}
