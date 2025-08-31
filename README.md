# IPC Demo - Demonstração de Comunicação Entre Processos

Este projeto demonstra diferentes mecanismos de Inter-Process Communication (IPC) em sistemas Unix/Linux, incluindo **Pipes Anônimos**, **Sockets Locais** e **Memória Compartilhada**. O sistema possui um backend em C++ e um frontend em Python com interface gráfica.

## 🏗️ Arquitetura do Projeto

### Estrutura de Diretórios
```
projeto-ipc/
├── src/
│   ├── backend/           # Implementações em C dos mecanismos IPC
│   │   ├── common/        # Código compartilhado (JSON output)
│   │   ├── pipes/         # Demonstração de pipes anônimos
│   │   ├── sockets/       # Demonstração de sockets locais
│   │   └── shared_memory/ # Demonstração de memória compartilhada
│   └── frontend/          # Interface gráfica em Python
│       ├── gui/           # Componentes da interface
│       └── backend_comm/  # Comunicação com o backend
├── tests/                 # Testes unitários e de integração
├── build/                 # Arquivos compilados (gerado)
├── CMakeLists.txt         # Configuração do CMake
└── build.sh              # Script de build automatizado
```

### Componentes Principais

#### Backend (C)
- **Pipes Anônimos** (`pipe_demo`): Comunicação bidirecional entre processo pai e filho
- **Sockets Locais** (`socket_demo`): Comunicação cliente-servidor via Unix domain sockets
- **Memória Compartilhada** (`shm_demo`): Compartilhamento de dados entre processos com sincronização via semáforos
- **JSON Output** (`json_output`): Sistema de logging estruturado para integração com frontend

#### Frontend (Python)
- **Interface Gráfica**: Aplicação Tkinter com abas para cada mecanismo IPC
- **Gerenciador de Processos**: Execução e monitoramento dos executáveis do backend
- **Parser JSON**: Interpretação das mensagens estruturadas do backend

## 🔧 Compilação e Execução

### Pré-requisitos

#### Sistema
- Linux/Unix (testado em WSL2)
- GCC (versão 7.0 ou superior)
- CMake (versão 3.10 ou superior)
- Make

#### Python
- Python 3.7+
- Tkinter (geralmente incluído com Python)

### Compilação do Backend

#### Opção 1: Script Automatizado (Recomendado)
```bash
# Dar permissão de execução ao script
chmod +x build.sh

# Executar build completo
./build.sh
```

#### Opção 2: Comandos Manuais
```bash
# 1. Criar diretório de build
mkdir -p build
cd build

# 2. Configurar com CMake
cmake ..

# 3. Compilar
make -j$(nproc)

# 4. Executar testes
ctest

# 5. Voltar ao diretório raiz
cd ..
```

### Execução do Frontend

```bash
# Executar a aplicação principal
python3 src/frontend/main.py
```

### Execução Individual dos Módulos

```bash
# Pipes
./build/pipe_demo "Sua mensagem aqui"

# Sockets
./build/socket_demo "Sua mensagem aqui"

# Memória Compartilhada
./build/shm_demo "Sua mensagem aqui"
```

## 📡 Protocolo de Comunicação

### Formato JSON

O backend utiliza um formato JSON padronizado para todas as mensagens:

#### Estrutura de Status
```json
{
  "type": "status",
  "module": "nome_do_modulo",
  "status": "tipo_status",
  "message": "descrição_detalhada",
  "pid": 12345,
  "timestamp": 1703123456
}
```

#### Estrutura de Dados
```json
{
  "type": "data",
  "module": "nome_do_modulo",
  "data": "conteúdo_dos_dados",
  "source": "origem_dos_dados",
  "pid": 12345,
  "timestamp": 1703123456
}
```

#### Estrutura de Erro
```json
{
  "type": "error",
  "module": "nome_do_modulo",
  "message": "descrição_do_erro",
  "pid": 12345,
  "timestamp": 1703123456
}
```

### Módulos de Comunicação

#### Pipes Anônimos
- **Funcionamento**: Cria dois pipes para comunicação bidirecional
- **Processo**: Pai envia mensagem → Filho recebe e ecoa → Pai recebe eco
- **Saída**: Logs de criação, comunicação e finalização

#### Sockets Locais
- **Funcionamento**: Servidor aguarda conexão, cliente envia dados
- **Processo**: Servidor aceita conexão → Cliente envia mensagem → Servidor ecoa
- **Saída**: Logs de conexão, recebimento e resposta

#### Memória Compartilhada
- **Funcionamento**: Segmento de memória compartilhado com sincronização via semáforos
- **Processo**: Pai escreve → Libera semáforo → Filho lê → Limpa recursos
- **Saída**: Logs de criação, escrita, sincronização e leitura

## 🧪 Testes

### Executar Todos os Testes
```bash
# Testes do backend
cd build && ctest && cd ..

# Testes do frontend
python tests/frontend_tests/integration.py
```

### Testes Individuais
```bash
# Teste de memória compartilhada
./build/shm_test

# Teste de JSON output
./build/json_output_test

# Teste de pipes
./build/pipe_test

# Teste de sockets
./build/socket_test
```

## 🚀 Funcionalidades

### Interface Gráfica
- **Aba Pipes**: Demonstração interativa de pipes anônimos
- **Aba Sockets**: Teste de comunicação via sockets locais
- **Aba Shared Memory**: Experimentação com memória compartilhada
- **Logs em Tempo Real**: Visualização das mensagens JSON do backend
- **Controles Interativos**: Campos de entrada e botões para cada teste

### Recursos do Backend
- **Logging Estruturado**: Todas as operações são registradas em JSON
- **Tratamento de Erros**: Captura e reporta erros de forma consistente
- **Sincronização**: Uso apropriado de semáforos e wait() para coordenação
- **Limpeza de Recursos**: Liberação adequada de memória e descritores

## 🔍 Solução de Problemas

### Erros Comuns

#### "Executáveis não encontrados"
```bash
# Verificar se o build foi executado
ls -la build/

# Recompilar se necessário
./build.sh
```

#### "Falha ao criar socket"
```bash
# Verificar se o arquivo de socket já existe
ls -la /tmp/ipc_socket

# Remover manualmente se necessário
rm -f /tmp/ipc_socket
```

#### "Erro de permissão"
```bash
# Dar permissão de execução
chmod +x build.sh
chmod +x build/*

# Verificar permissões do diretório
ls -la build/
```

### Logs e Debug
- Todas as operações são logadas em JSON no stdout
- Use `2>&1` para capturar erros junto com a saída normal
- Verifique os logs do frontend para mensagens de erro

## 📚 Dependências

### Bibliotecas do Sistema
- `libc` (padrão)
- `librt` (para shared memory e semáforos)
- `libpthread` (para operações de processo)

### Módulos Python
- `tkinter` (interface gráfica)
- `subprocess` (execução de processos)
- `json` (parsing de mensagens)
- `os` e `sys` (operações do sistema)

## 🤝 Contribuição

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

## 👥 Autores

- **Seu Nome** - *Desenvolvimento inicial* - [SeuGitHub](https://github.com/seugithub)

## 🙏 Agradecimentos

- Professores e colegas do curso de Sistemas Operacionais
- Comunidade Linux/Unix
- Documentação do CMake e Python
