import tkinter as tk
from tkinter import ttk, scrolledtext
from backend_comm.process_manager import BackendManager
import json
from datetime import datetime

class IPCDemoWindow:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("IPC Demo - Comunicação Entre Processos")
        self.root.geometry("800x600")
        
        self.backend_manager = BackendManager()
        self.setup_ui()
        
        # Cleanup ao fechar
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
    
    def setup_ui(self):
        """Configura a interface do usuário"""
        # Notebook para abas
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Aba Pipes
        self.setup_pipes_tab()
        
        # Aba Sockets
        self.setup_sockets_tab()
        
        # Aba Shared Memory
        self.setup_shm_tab()
    
    def setup_pipes_tab(self):
        """Configura aba de demonstração de pipes"""
        pipes_frame = ttk.Frame(self.notebook)
        self.notebook.add(pipes_frame, text="Pipes Anônimos")
        
        # Frame superior - controles
        control_frame = ttk.Frame(pipes_frame)
        control_frame.pack(fill="x", padx=5, pady=5)
        
        ttk.Label(control_frame, text="Mensagem:").pack(side="left")
        
        self.pipes_message = tk.StringVar(value="Hello from pipe!")
        entry = ttk.Entry(control_frame, textvariable=self.pipes_message, width=30)
        entry.pack(side="left", padx=5)
        
        ttk.Button(control_frame, text="Enviar", 
                  command=self.send_pipe_message).pack(side="left", padx=5)
        
        ttk.Button(control_frame, text="Limpar", 
                  command=lambda: self.pipes_output.delete(1.0, "end")).pack(side="left")
        
        # Frame inferior - saída
        ttk.Label(pipes_frame, text="Saída:").pack(anchor="w", padx=5)
        
        self.pipes_output = scrolledtext.ScrolledText(pipes_frame, height=20)
        self.pipes_output.pack(fill="both", expand=True, padx=5, pady=5)
    
    def setup_sockets_tab(self):
        """Configura aba de demonstração de sockets"""
        sockets_frame = ttk.Frame(self.notebook)
        self.notebook.add(sockets_frame, text="Sockets Locais")
        
        # Implementação similar aos pipes
        ttk.Label(sockets_frame, text="Implementação de Sockets - Em desenvolvimento").pack(pady=20)
    
    def setup_shm_tab(self):
        """Configura aba de demonstração de memória compartilhada"""
        shm_frame = ttk.Frame(self.notebook)
        self.notebook.add(shm_frame, text="Memória Compartilhada")
        
        # Implementação similar aos pipes
        ttk.Label(shm_frame, text="Implementação de Memória Compartilhada - Em desenvolvimento").pack(pady=20)
    
    def send_pipe_message(self):
        """Envia mensagem via pipe"""
        message = self.pipes_message.get()
        if message:
            self.backend_manager.start_process(
                "pipes",
                "pipe_demo",
                [message],
                self.on_pipes_output
            )
    
    def on_pipes_output(self, data):
        """Callback para saída dos pipes"""
        self.root.after(0, lambda: self.update_pipes_display(data))
    
    def update_pipes_display(self, data):
        """Atualiza display dos pipes (thread-safe)"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        
        if data["type"] == "status":
            text = f"[{timestamp}] STATUS: {data['message']}\n"
            self.pipes_output.insert("end", text)
            
        elif data["type"] == "data":
            text = f"[{timestamp}] {data['source'].upper()}: {data['data']}\n"
            self.pipes_output.insert("end", text)
            
        elif data["type"] == "error":
            text = f"[{timestamp}] ERRO: {data['error']}\n"
            self.pipes_output.insert("end", text)
        
        # Auto-scroll para baixo
        self.pipes_output.see("end")
    
    def on_close(self):
        """Cleanup ao fechar aplicação"""
        self.backend_manager.stop_all()
        self.root.destroy()
    
    def run(self):
        """Inicia a aplicação"""
        self.root.mainloop()