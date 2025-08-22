# backend_comm/process_manager.py
import subprocess
import json
import threading
import queue
from typing import Optional, Callable

class BackendManager:
    def __init__(self):
        self.processes = {}
        self.output_queues = {}
        self.callbacks = {}
    
    def start_process(self, module: str, executable: str, args: list, 
                     callback: Callable[[dict], None]):
        """Inicia um processo C e configura comunicação"""
        if module in self.processes:
            self.stop_process(module)
        
        try:
            # Iniciar processo
            process = subprocess.Popen(
                [f"./build/{executable}"] + args,
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
            self.callbacks[module]({
                "type": "error",
                "module": module,
                "error": f"Failed to start process: {str(e)}"
            })
            return False
    
    def _read_output(self, module: str):
        """Lê saída JSON do processo C"""
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
        """Para um processo específico"""
        if module in self.processes:
            self.processes[module].terminate()
            del self.processes[module]
            if module in self.output_queues:
                del self.output_queues[module]
            if module in self.callbacks:
                del self.callbacks[module]
    
    def stop_all(self):
        """Para todos os processos"""
        for module in list(self.processes.keys()):
            self.stop_process(module)