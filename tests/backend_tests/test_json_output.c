#include <stdio.h>
#include <string.h>
#include <unistd.h> // Para getpid()
#include "json_output.h"

// Função de apoio para testar uma chamada e mostrar no stdout o resultado esperado e obtido
void test_print_json_status() {
    printf("=== Teste: print_json_status ===\n");
    // Esperado:
    // {"type":"status","module":"pipes","status":"ok","message":"Tudo certo","pid":12345,"timestamp":algum_numero}
    print_json_status("pipes", "ok", "Tudo certo", getpid());
}

void test_print_json_data() {
    printf("=== Teste: print_json_data ===\n");
    print_json_data("pipes", "mensagem", "pai", getpid());
}

void test_print_json_error() {
    printf("=== Teste: print_json_error ===\n");
    print_json_error("pipes", "Erro qualquer", getpid());
}

int main() {
    test_print_json_status();
    test_print_json_data();
    test_print_json_error();
    return 0;
}

