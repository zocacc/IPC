#include "json_output.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// Função auxiliar para escapar strings JSON
char* escape_json_string(const char* input) {
    if (!input) return NULL;
    
    int len = strlen(input);
    int escaped_len = len;
    
    // Conta caracteres que precisam ser escapados
    for (int i = 0; i < len; i++) {
        if (input[i] == '"' || input[i] == '\\' || input[i] == '\n' || input[i] == '\r' || input[i] == '\t') {
            escaped_len += 1;
        }
    }
    
    char* escaped = malloc(escaped_len + 1);
    if (!escaped) return NULL;
    
    int j = 0;
    for (int i = 0; i < len; i++) {
        switch (input[i]) {
            case '"':
                escaped[j++] = '\\';
                escaped[j++] = '"';
                break;
            case '\\':
                escaped[j++] = '\\';
                escaped[j++] = '\\';
                break;
            case '\n':
                escaped[j++] = '\\';
                escaped[j++] = 'n';
                break;
            case '\r':
                escaped[j++] = '\\';
                escaped[j++] = 'r';
                break;
            case '\t':
                escaped[j++] = '\\';
                escaped[j++] = 't';
                break;
            default:
                escaped[j++] = input[i];
                break;
        }
    }
    escaped[j] = '\0';
    return escaped;
}

void print_json_status(const char* module, const char* status, const char* message, int pid) {
    time_t now;
    time(&now);
    
    char* escaped_module = escape_json_string(module);
    char* escaped_status = escape_json_string(status);
    char* escaped_message = escape_json_string(message);
    
    if (pid > 0) {
        printf("{\"type\":\"status\",\"module\":\"%s\",\"status\":\"%s\",\"message\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_status ? escaped_status : status, 
               escaped_message ? escaped_message : message, 
               pid, now);
    } else {
        printf("{\"type\":\"status\",\"module\":\"%s\",\"status\":\"%s\",\"message\":\"%s\",\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_status ? escaped_status : status, 
               escaped_message ? escaped_message : message, 
               now);
    }
    fflush(stdout);
    
    // Libera memória alocada
    free(escaped_module);
    free(escaped_status);
    free(escaped_message);
}

void print_json_data(const char* module, const char* data, const char* source, int pid) {
    time_t now;
    time(&now);
    
    char* escaped_module = escape_json_string(module);
    char* escaped_data = escape_json_string(data);
    char* escaped_source = escape_json_string(source);
    
    if (pid > 0) {
        printf("{\"type\":\"data\",\"module\":\"%s\",\"data\":\"%s\",\"source\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_data ? escaped_data : data, 
               escaped_source ? escaped_source : source, 
               pid, now);
    } else {
        printf("{\"type\":\"data\",\"module\":\"%s\",\"data\":\"%s\",\"source\":\"%s\",\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_data ? escaped_data : data, 
               escaped_source ? escaped_source : source, 
               now);
    }
    fflush(stdout);
    
    // Libera memória alocada
    free(escaped_module);
    free(escaped_data);
    free(escaped_source);
}

void print_json_error(const char* module, const char* error, int pid) {
    time_t now;
    time(&now);
    
    char* escaped_module = escape_json_string(module);
    char* escaped_error = escape_json_string(error);
    
    if (pid > 0) {
        printf("{\"type\":\"error\",\"module\":\"%s\",\"error\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_error ? escaped_error : error, 
               pid, now);
    } else {
        printf("{\"type\":\"error\",\"module\":\"%s\",\"error\":\"%s\",\"timestamp\":%ld}\n", 
               escaped_module ? escaped_module : module, 
               escaped_error ? escaped_error : error, 
               now);
    }
    fflush(stdout);
    
    // Libera memória alocada
    free(escaped_module);
    free(escaped_error);
}
