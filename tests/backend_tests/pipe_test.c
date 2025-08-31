#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para getpid()
#include "json_output.h"

#define BUFFER_SIZE 4096

// Function to execute a command and capture its output
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

void run_pipe_test() {
    const char* executable = "./build/pipe_demo";
    const char* test_message = "hello_from_ctest";
    char command[512];
    snprintf(command, sizeof(command), "%s %s", executable, test_message);

    char* output = execute_command(command);

    if (!output) {
        print_json_error("pipe_test", "Failed to execute pipe_demo", getpid());
        return;
    }

    // Exibe o JSON real que o frontend receberia
    printf("%s", output);

    // Verificações mais robustas do JSON
    int status_ok = 1;
    // Verifica se a mensagem do pai foi enviada e contém o PID
    if (strstr(output, "\"type\":\"data\"") == NULL || 
        strstr(output, "\"source\":\"pai -> filho\"") == NULL ||
        strstr(output, test_message) == NULL ||
        strstr(output, "\"pid\":") == NULL) {
        status_ok = 0;
    }
    
    // Verifica se o eco do filho foi recebido e contém o PID
    if (strstr(output, "\"source\":\"filho -> pai (eco)\"") == NULL) {
        status_ok = 0;
    }

    if (status_ok) {
        print_json_status("pipe_test", "test_pass", "Pipe test completed successfully.", getpid());
    } else {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Pipe test failed. Full output:\n%s", output);
        print_json_error("pipe_test", error_msg, getpid());
    }

    free(output);
}

int main() {
    run_pipe_test();
    return 0;
}
