# Guia de Uso - IPC Demo

Este documento fornece instruções detalhadas sobre como usar o sistema IPC Demo, incluindo exemplos práticos e casos de uso.

## 🚀 Início Rápido

### 1. Compilação e Execução
```bash
# Clone o repositório (se aplicável)
git clone <url-do-repositorio>
cd projeto-ipc

# Compile o projeto
./build.sh

# Execute o frontend
python3 src/frontend/main.py
```

### 2. Teste Individual dos Módulos
```bash
# Teste de pipes
./build/pipe_demo "Sua mensagem de teste"

# Teste de sockets
./build/socket_demo "Mensagem via socket"

# Teste de memória compartilhada
./build/shm_demo "Dados compartilhados"
```

## 🖥️ Interface Gráfica

### Aba Pipes
- **Campo de Mensagem**: Digite a mensagem que será enviada via pipes
- **Botão Enviar**: Executa a demonstração de pipes anônimos
- **Botão Limpar**: Remove todo o conteúdo da área de saída
- **Área de Saída**: Mostra logs JSON em tempo real

### Aba Sockets
- **Campo de Mensagem**: Mensagem para enviar via sockets locais
- **Botão Enviar**: Inicia servidor e cliente para demonstração
- **Botão Limpar**: Limpa a área de saída
- **Área de Saída**: Logs de conexão e comunicação

### Aba Shared Memory
- **Campo de Mensagem**: Dados para compartilhar via memória
- **Botão Enviar**: Demonstra sincronização via semáforos
- **Botão Limpar**: Limpa a área de saída
- **Área de Saída**: Logs de criação, escrita e leitura

## 📊 Interpretando a Saída JSON

### Exemplo de Mensagem de Status
```json
{
  "type": "status",
  "module": "pipes",
  "status": "info",
  "message": "Iniciando a demonstração de pipes.",
  "pid": 12345,
  "timestamp": 1703123456
}
```

**Campos:**
- `type`: Sempre "status" para mensagens de estado
- `module`: Módulo que gerou a mensagem
- `status`: Tipo de status (info, success, warning, error)
- `message`: Descrição detalhada
- `pid`: ID do processo (0 se não aplicável)
- `timestamp`: Timestamp Unix em segundos

### Exemplo de Mensagem de Dados
```json
{
  "type": "data",
  "module": "pipes",
  "data": "Hello World",
  "source": "pai -> filho",
  "pid": 12345,
  "timestamp": 1703123457
}
```

**Campos:**
- `type`: Sempre "data" para transmissão de dados
- `module`: Módulo que gerou a mensagem
- `data`: Conteúdo dos dados transmitidos
- `source`: Origem dos dados
- `pid`: ID do processo
- `timestamp`: Timestamp Unix

### Exemplo de Mensagem de Erro
```json
{
  "type": "error",
  "module": "pipes",
  "message": "Falha ao criar os pipes.",
  "pid": 12345,
  "timestamp": 1703123458
}
```

**Campos:**
- `type`: Sempre "error" para mensagens de erro
- `module`: Módulo que gerou o erro
- `message`: Descrição detalhada do erro
- `pid`: ID do processo
- `timestamp`: Timestamp Unix

## 🔧 Casos de Uso Avançados

### 1. Integração com Scripts
```bash
#!/bin/bash
# script_exemplo.sh

echo "Testando todos os módulos IPC..."

# Teste de pipes
echo "=== Teste de Pipes ==="
./build/pipe_demo "Mensagem de teste" 2>&1 | grep "type.*data"

# Teste de sockets
echo "=== Teste de Sockets ==="
./build/socket_demo "Teste socket" 2>&1 | grep "type.*status"

# Teste de memória compartilhada
echo "=== Teste de Shared Memory ==="
./build/shm_demo "Dados compartilhados" 2>&1 | grep "type.*data"
```

### 2. Filtragem de Logs
```bash
# Filtrar apenas mensagens de dados
./build/pipe_demo "Teste" | grep '"type":"data"'

# Filtrar mensagens de um módulo específico
./build/socket_demo "Teste" | grep '"module":"socket_demo"'

# Filtrar mensagens de erro
./build/shm_demo "Teste" | grep '"type":"error"'
```

### 3. Análise de Performance
```bash
# Medir tempo de execução
time ./build/pipe_demo "Mensagem de teste"

# Contar mensagens por tipo
./build/socket_demo "Teste" | jq -r '.type' | sort | uniq -c

# Analisar latência entre mensagens
./build/shm_demo "Teste" | jq -r '.timestamp' | awk 'NR>1{print $1-p} {p=$1}'
```

## 🧪 Executando Testes

### Testes Automatizados
```bash
# Executar todos os testes do backend
cd build && ctest --verbose && cd ..

# Executar testes específicos
./build/shm_test
./build/json_output_test
./build/pipe_test
./build/socket_test
```

### Testes de Integração
```bash
# Teste do frontend
python tests/frontend_tests/integration.py

# Teste manual de comunicação
python3 -c "
import subprocess
import json

# Executar pipe_demo e capturar saída
result = subprocess.run(['./build/pipe_demo', 'Teste'], 
                       capture_output=True, text=True)

# Parsear saída JSON
for line in result.stdout.splitlines():
    if line.strip():
        try:
            data = json.loads(line)
            print(f'Received: {data[\"type\"]} from {data[\"module\"]}')
        except json.JSONDecodeError:
            print(f'Raw output: {line}')
"
```

## 🔍 Debug e Solução de Problemas

### Verificar Dependências
```bash
# Verificar compilador
gcc --version

# Verificar CMake
cmake --version

# Verificar Python
python3 --version

# Verificar bibliotecas do sistema
ldd ./build/pipe_demo
```

### Logs de Sistema
```bash
# Ver logs do kernel para IPC
dmesg | grep -i ipc

# Ver processos ativos
ps aux | grep -E "(pipe_demo|socket_demo|shm_demo)"

# Ver arquivos de socket
ls -la /tmp/ipc_socket*

# Ver segmentos de memória compartilhada
ipcs -m
```

### Limpeza de Recursos
```bash
# Remover arquivos de socket órfãos
sudo rm -f /tmp/ipc_socket*

# Limpar segmentos de memória compartilhada
sudo ipcrm -M 0x12345678  # Substitua pelo ID correto

# Remover semáforos órfãos
sudo ipcrm -S 0x12345678  # Substitua pelo ID correto
```

## 📈 Monitoramento em Tempo Real

### Usando watch
```bash
# Monitorar processos IPC
watch -n 1 'ps aux | grep -E "(pipe_demo|socket_demo|shm_demo)"'

# Monitorar arquivos de socket
watch -n 1 'ls -la /tmp/ipc_socket*'

# Monitorar segmentos de memória
watch -n 1 'ipcs -m'
```

### Usando strace
```bash
# Rastrear chamadas de sistema
strace -f ./build/pipe_demo "Teste"

# Rastrear apenas chamadas IPC
strace -f -e trace=ipc ./build/shm_demo "Teste"
```

## 🎯 Exemplos de Configuração

### Personalizar Tamanho de Buffer
```c
// Em src/backend/common/json_output.c
#define BUFFER_SIZE 512  // Aumentar de 256 para 512
```

### Personalizar Caminho de Socket
```c
// Em src/backend/sockets/socket_demo.h
#define SOCKET_PATH "/tmp/meu_socket_customizado.sock"
```

### Personalizar Tamanho de Memória Compartilhada
```c
// Em src/backend/shared_memory/shm_handler.h
#define SHM_SIZE 8192  // Aumentar de 4096 para 8192 bytes
```

## 📚 Recursos Adicionais

### Documentação do Sistema
- `man 2 pipe` - Documentação sobre pipes
- `man 2 socket` - Documentação sobre sockets
- `man 2 shmget` - Documentação sobre memória compartilhada
- `man 3 sem_init` - Documentação sobre semáforos

### Tutoriais Online
- [Linux IPC Tutorial](https://tldp.org/LDP/lpg/node7.html)
- [POSIX IPC Guide](https://man7.org/linux/man-pages/man7/svipc.7.html)
- [Unix Domain Sockets](https://man7.org/linux/man-pages/man7/unix.7.html)

### Ferramentas de Análise
- `strace` - Rastreamento de chamadas de sistema
- `ltrace` - Rastreamento de chamadas de biblioteca
- `valgrind` - Análise de memória
- `gdb` - Debugger GNU
