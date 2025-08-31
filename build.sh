#!/bin/bash
# build.sh - Script simples de build para o projeto IPC Demo

# Garante que o script pare se algum comando falhar
set -e

echo "=== IPC Demo - Script de Build ==="
echo

# 1. Limpar e criar diretório de build
echo "1. Preparando diretório de build..."
rm -rf build
mkdir -p build
echo "✓ Diretório build criado"

# 2. Configurar com CMake
echo "2. Configurando projeto com CMake..."
cmake -S . -B build
echo "✓ CMake configurado"

# 3. Compilar
echo "3. Compilando backend..."
make -C build -j$(nproc)
echo "✓ Backend compilado"

# 4. Verificar executáveis
echo "4. Verificando executáveis..."
executables=("pipe_demo" "socket_demo" "shm_demo")
for exe in "${executables[@]}"; do
    if [ -f "build/$exe" ]; then
        echo "✓ $exe encontrado"
    else
        echo "✗ $exe não encontrado"
        exit 1
    fi
done

# 5. Executar testes
echo "5. Executando testes..."
cd build && ctest && cd ..
echo "✓ Testes executados"

echo
echo "=== Build concluído com sucesso! ==="
echo

# Menu simples
while true; do
    echo "O que você gostaria de fazer?"
    echo "1. Executar frontend"
    echo "2. Testar pipes"
    echo "3. Testar sockets"
    echo "4. Testar shared memory"
    echo "5. Testar tudo"
    echo "6. Sair"
    echo
    read -p "Escolha uma opção (1-6): " choice
    
    case $choice in
        1)
            echo "Executando frontend..."
            python3 src/frontend/main.py
            ;;
        2)
            echo "Testando pipes..."
            ./build/pipe_demo "Teste de pipes"
            ;;
        3)
            echo "Testando sockets..."
            ./build/socket_demo "Teste de sockets"
            ;;
        4)
            echo "Testando shared memory..."
            ./build/shm_demo "Teste de shared memory"
            ;;
                 5)
             echo "Testando tudo..."
             echo "Pipes:"
             ./build/pipe_demo "Teste completo"
             echo "Sockets:"
             ./build/socket_demo "Teste completo"
             echo "Shared Memory:"
             ./build/shm_demo "Teste completo"
             echo "Frontend (teste de integração):"
             if [ -f "tests/frontend_tests/integration.py" ]; then
                 python3 tests/frontend_tests/integration.py
             else
                 echo "Arquivo de teste de integração não encontrado"
             fi
             ;;
        6)
            echo "Saindo..."
            exit 0
            ;;
        *)
            echo "Opção inválida"
            ;;
    esac
    
    echo
    echo "Pressione Enter para continuar..."
    read
    echo
done