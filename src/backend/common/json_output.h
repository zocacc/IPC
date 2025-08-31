/**
 * @file json_output.h
 * @brief Sistema de logging estruturado em formato JSON para o projeto IPC Demo
 * 
 * Este arquivo define funções para gerar saída JSON padronizada que permite
 * a integração entre o backend C e o frontend Python. Todas as mensagens
 * seguem um formato consistente com campos obrigatórios.
 * 
 * @author [Seu Nome]
 * @date [Data de Criação]
 */

#ifndef JSON_OUTPUT_H
#define JSON_OUTPUT_H

/**
 * @brief Imprime uma mensagem de status em formato JSON
 * 
 * Gera uma mensagem JSON com informações sobre o status de uma operação.
 * Útil para logging de eventos como início, fim, ou mudanças de estado.
 * 
 * @param module Nome do módulo que está gerando a mensagem (ex: "pipes", "sockets")
 * @param status Tipo de status (ex: "info", "success", "warning")
 * @param message Descrição detalhada do status
 * @param pid ID do processo (use 0 se não aplicável)
 * 
 * @note A função automaticamente escapa caracteres especiais na mensagem
 * @note Timestamp é gerado automaticamente no momento da chamada
 * 
 * @example
 * print_json_status("pipes", "info", "Iniciando demonstração de pipes", getpid());
 */
void print_json_status(const char* module, const char* status, const char* message, int pid);

/**
 * @brief Imprime dados em formato JSON
 * 
 * Gera uma mensagem JSON contendo dados específicos de uma operação.
 * Usado para transmitir informações como mensagens recebidas, dados processados, etc.
 * 
 * @param module Nome do módulo que está gerando a mensagem
 * @param data Conteúdo dos dados a serem transmitidos
 * @param source Origem dos dados (ex: "pai -> filho", "cliente -> servidor")
 * @param pid ID do processo (use 0 se não aplicável)
 * 
 * @note A função automaticamente escapa caracteres especiais nos dados
 * @note Timestamp é gerado automaticamente no momento da chamada
 * 
 * @example
 * print_json_data("pipes", "Hello World", "pai -> filho", getpid());
 */
void print_json_data(const char* module, const char* data, const char* source, int pid);

/**
 * @brief Imprime uma mensagem de erro em formato JSON
 * 
 * Gera uma mensagem JSON para reportar erros durante a execução.
 * Usado para logging de falhas, exceções ou condições de erro.
 * 
 * @param module Nome do módulo que está gerando a mensagem
 * @param error Descrição detalhada do erro
 * @param pid ID do processo (use 0 se não aplicável)
 * 
 * @note A função automaticamente escapa caracteres especiais na mensagem
 * @note Timestamp é gerado automaticamente no momento da chamada
 * 
 * @example
 * print_json_error("pipes", "Falha ao criar pipe", getpid());
 */
void print_json_error(const char* module, const char* error, int pid);

#endif // JSON_OUTPUT_H