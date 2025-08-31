/**
 * @file socket_demo.h
 * @brief Configurações para demonstração de sockets locais (Unix domain sockets)
 * 
 * Este arquivo define constantes e configurações específicas para a
 * demonstração de comunicação entre processos usando sockets locais.
 * 
 * @author [Seu Nome]
 * @date [Data de Criação]
 */

#ifndef SOCKET_DEMO_H
#define SOCKET_DEMO_H

/**
 * @brief Caminho para o arquivo de socket local
 * 
 * Define o caminho onde o arquivo de socket será criado para
 * comunicação entre processos. Usar /tmp/ é uma prática comum
 * para arquivos temporários que não precisam persistir entre
 * reinicializações do sistema.
 * 
 * @note O arquivo de socket é criado automaticamente quando o
 *       servidor faz bind() e removido quando o programa termina
 * @note Em sistemas Unix/Linux, este arquivo aparece como um
 *       arquivo especial no sistema de arquivos
 * @note O diretório /tmp/ deve ter permissões adequadas para
 *       criação de arquivos pelo usuário executando o programa
 */
#define SOCKET_PATH "/tmp/ipc_socket_demo.sock"

#endif // SOCKET_DEMO_H
