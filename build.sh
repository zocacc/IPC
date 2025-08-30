#!/bin/bash
# build.sh

# Garante que o script pare se algum comando falhar
set -e

echo "=== Limpando e recriando o diretório de build... "
rm -rf build
mkdir -p build

echo "=== Configurando o projeto com CMake... "
cmake -S . -B build

echo "=== Compilando o backend C com Make... "
# O -C build diz ao make para executar a partir do diretório 'build'
make -C build -j$(nproc)

echo "=== Instalando dependências Python (se necessário)... "
# Descomente a linha abaixo se não estiver em um ambiente virtual ativado
# pip install -r requirements.txt

echo -e "\n=== Build concluído! "
echo "Para executar a aplicação, rode:"
echo "python3 src/frontend/main.py"
