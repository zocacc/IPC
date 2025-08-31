# backend_comm/process_manager.py
"""
Gerenciador de processos para comunicação com o backend C.

Este módulo fornece uma interface Python para executar e gerenciar
processos do backend C, incluindo captura de saída JSON e callbacks
para processamento das mensagens em tempo real.

@author [Seu Nome]
@date [Data de Criação]
"""

import subprocess
import json
import threading
import queue
import os
from typing import Optional, Callable

class BackendManager:
    """
    Gerenciador de processos do backend C com suporte a comunicação JSON.
    
    Esta classe gerencia a execução de processos C compilados, captura
    sua saída JSON e fornece callbacks para processamento das mensagens.
    Suporta múltiplos módulos simultâneos (pipes, sockets, shared memory).
    
    Attributes:
        processes (dict): Mapeia nomes de módulos para objetos subprocess.Popen
        output_queues (dict): Filas de saída para cada módulo (não usado atualmente)
        callbacks (dict): Funções de callback para processar mensagens de cada módulo
    """
    
    def __init__(self):
        """Inicializa o gerenciador de processos."""
        self.processes = {}
        self.output_queues = {}
        self.callbacks = {}
    
    def start_process(self, module: str, executable: str, args: list, 
                     callback: Callable[[dict], None]) -> bool:
        """
        Inicia um processo C e configura comunicação JSON.
        
        Cria um novo processo executando o binário C especificado,
        configura captura de saída e inicia uma thread para ler
        as mensagens JSON em tempo real.
        
        Args:
            module: Nome identificador do módulo (ex: 'pipes', 'sockets')
            executable: Nome do executável no diretório build/
            args: Lista de argumentos para passar ao executável
            callback: Função chamada para cada mensagem JSON recebida
            
        Returns:
            bool: True se o processo foi iniciado com sucesso, False caso contrário
            
        Note:
            Se um processo com o mesmo módulo já estiver rodando,
            ele será terminado antes de iniciar o novo.
            
        Example:
            def handle_message(data):
                print(f"Received: {data}")
            
            success = manager.start_process(
                "pipes", "pipe_demo", ["Hello World"], handle_message
            )
        """
        if module in self.processes:
            self.stop_process(module)
        
        try:
            # Verificar se o executável existe
            executable_path = f"./build/{executable}"
            if not os.path.exists(executable_path):
                callback({
                    "type": "error",
                    "module": module,
                    "error": f"Executable not found: {executable_path}"
                })
                return False
            
            # Iniciar processo
            process = subprocess.Popen(
                [executable_path] + args,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1
            )
            
            self.processes[module] = process
            self.output_queues[module] = queue.Queue()
            self.callbacks[module] = callback
            
            # Thread para ler saída
            output_thread = threading.Thread(
                target=self._read_output, 
                args=(module,),
                daemon=True
            )
            output_thread.start()
            
            return True
            
        except Exception as e:
            # Chamar o callback diretamente em caso de erro na inicialização
            callback({
                "type": "error",
                "module": module,
                "error": f"Failed to start process: {str(e)}"
            })
            return False
    
    def _read_output(self, module: str):
        """
        Lê saída JSON do processo C em uma thread separada.
        
        Esta função roda em uma thread dedicada para cada módulo,
        lendo continuamente a saída stdout do processo C e tentando
        fazer parse das mensagens JSON. Mensagens não-JSON válidas
        são convertidas para formato raw.
        
        Args:
            module: Nome do módulo cuja saída deve ser lida
            
        Note:
            Esta é uma função interna chamada automaticamente
            quando start_process() é executado.
        """
        process = self.processes[module]
        
        try:
            for line in process.stdout:
                line = line.strip()
                if line:
                    try:
                        data = json.loads(line)
                        self.callbacks[module](data)
                    except json.JSONDecodeError:
                        # Linha não é JSON válido
                        self.callbacks[module]({
                            "type": "raw",
                            "module": module,
                            "data": line
                        })
        except Exception as e:
            self.callbacks[module]({
                "type": "error",
                "module": module,
                "error": f"Output reading error: {str(e)}"
            })
    
    def stop_process(self, module: str):
        """
        Para um processo específico e limpa recursos associados.
        
        Termina o processo especificado e remove todas as referências
        a ele das estruturas internas do gerenciador.
        
        Args:
            module: Nome do módulo cujo processo deve ser parado
            
        Note:
            O processo é terminado graciosamente usando terminate().
            Se necessário, pode ser forçado com kill().
        """
        if module in self.processes:
            self.processes[module].terminate()
            del self.processes[module]
            if module in self.output_queues:
                del self.output_queues[module]
            if module in self.callbacks:
                del self.callbacks[module]
    
    def stop_all(self):
        """
        Para todos os processos ativos e limpa todos os recursos.
        
        Útil para limpeza ao fechar a aplicação ou reiniciar
        todos os módulos simultaneamente.
        """
        for module in list(self.processes.keys()):
            self.stop_process(module)