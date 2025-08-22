#include <stdio.h>
#include <string.h>
#include "json_output.h"

// Função de apoio para testar uma chamada e mostrar no stdout o resultado esperado e obtido
void test_print_json_status() {
    printf("=== Teste: print_json_status ===\n");
    // Esperado:
    // {"type":"status","module":"pipes","status":"ok","message":"Tudo certo","timestamp":algum_numero}
    print_json_status("pipes", "ok", "Tudo certo");
}

void test_print_json_data() {
    printf("=== Teste: print_json_data ===\n");
    print_json_data("pipes", "mensagem", "pai");
}

void test_print_json_error() {
    printf("=== Teste: print_json_error ===\n");
    print_json_error("pipes", "Erro qualquer");
}

int main() {
    test_print_json_status();
    test_print_json_data();
    test_print_json_error();
    return 0;
}
