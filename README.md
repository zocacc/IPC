
```markdown
# IPC Demo - Comunicação Entre Processos

## Compilação

```bash
chmod +x build.sh
./build.sh
```

## Execução

```bash
cd src/frontend
python main.py
```

## Testes

```bash
cd build
./pipe_demo "teste"
./socket_demo
./shm_demo "dados"
```
```

## Fase 5: Cronograma de Desenvolvimento

### Semana 1: Base e Pipes
1. **Dia 1-2**: Estrutura do projeto + Módulo comum (json_output)
2. **Dia 3-4**: Implementação completa de pipes (C + Python)
3. **Dia 5**: Testes e refinamentos dos pipes
4. **Dia 6-7**: Interface básica funcionando com pipes

### Semana 2: Sockets e Shared Memory
1. **Dia 1-2**: Implementação de sockets
2. **Dia 3-4**: Implementação de memória compartilhada
3. **Dia 5**: Integração completa na interface
4. **Dia 6-7**: Testes finais e documentação

## Vantagens desta Abordagem

### Simplicidade para Aprendizado
- **C puro**: Conceitos de IPC mais claros
- **Python simples**: Interface rápida de desenvolver
- **JSON**: Comunicação clara e debuggável
- **Modularidade**: Cada parte pode ser testada independentemente

### Facilidade de Explicação
- **Separação clara**: Backend vs Frontend
- **Protocolo simples**: JSON via stdout
- **Logs visuais**: Cada operação aparece na interface
- **Código linear**: Fácil de seguir passo a passo

### Robustez
- **Tratamento de erros**: Em todas as camadas
- **Cleanup**: Processos são terminados corretamente
- **Threading**: Interface não trava
- **Cross-platform**: Funciona em Linux/macOS

## Dicas para Desenvolvimento

1. **Comece pequeno**: Implemente um pipe simples primeiro
2. **Teste constantemente**: Execute após cada mudança
3. **Debug visual**: Use prints abundantes no início
4. **Versioning**: Commit após cada funcionalidade
5. **Documentação**: Comente conforme desenvolve

Este guia oferece uma base sólida para aprender IPC enquanto cria algo funcional e demonstrável.