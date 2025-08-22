#!/bin/bash
# build.sh

set -e  # Parar em caso de erro

echo "=== Compilando backend C ==="
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..

echo "=== Instalando dependências Python ==="
pip install -r requirements.txt

echo "=== Build concluído! ==="
echo "Para executar: python src/frontend/main.py"