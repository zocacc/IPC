#include <gtest/gtest.h>
#include <string>
#include <cstdio>
#include <memory>
#include <array>
#include <iostream>
#include <stdexcept>

// Função auxiliar para executar um comando no shell e capturar sua saída
std::string exec(const char* cmd) {
    std::array<char, 256> buffer;
    std::string result;
    // Abre um pipe para ler a saída do comando executado
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    // Lê a saída linha por linha
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Define um conjunto de testes para o nosso PipeDemo
TEST(PipeDemoTest, BasicCommunicationFlow) {
    // 1. Preparação
    // Define o caminho para o executável e a mensagem de teste.
    // O teste assume que será executado a partir do diretório raiz do projeto.
    std::string executable = "./build/pipe_demo";
    std::string test_message = "hello_from_gtest";
    std::string command = executable + " " + test_message;

    // 2. Execução
    // Executa o comando e captura a saída.
    std::string output;
    try {
        output = exec(command.c_str());
    } catch (const std::runtime_error& e) {
        FAIL() << "Falha ao executar o pipe_demo: " << e.what();
    }

    // 3. Verificação (Asserts)
    // Verifica se a saída não está vazia.
    ASSERT_FALSE(output.empty()) << "A saída do pipe_demo estava vazia.";

    // Verifica se as mensagens de status chave estão presentes na saída,
    // o que confirma que o fluxo de execução ocorreu como esperado.
    EXPECT_NE(output.find("Iniciando a demonstração de pipes"), std::string::npos);
    EXPECT_NE(output.find("Pipes criados com sucesso"), std::string::npos);
    EXPECT_NE(output.find("Pai enviando mensagem para o filho"), std::string::npos);
    EXPECT_NE(output.find("Filho ecoando a mensagem para o pai"), std::string::npos);
    EXPECT_NE(output.find("Demonstração de pipes concluída"), std::string::npos);

    // Verifica se a mensagem enviada pelo pai foi logada corretamente.
    std::string parent_sent_log = "\"data\":\"" + test_message + "\",\"source\":\"pai -> filho\"";
    EXPECT_NE(output.find(parent_sent_log), std::string::npos)
        << "Não foi encontrado o log do pai enviando a mensagem.";

    // Verifica se o eco enviado pelo filho foi logado corretamente.
    std::string child_echo_log = "\"data\":\"" + test_message + "\",\"source\":\"filho -> pai (eco)\"";
    EXPECT_NE(output.find(child_echo_log), std::string::npos)
        << "Não foi encontrado o log do filho ecoando a mensagem.";
}

