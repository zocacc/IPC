import tkinter as tk
from tkinter import ttk, scrolledtext
import time

class IPCTabs:
    def __init__(self, notebook, backend_manager):
        self.notebook = notebook
        self.backend_manager = backend_manager
        
        # Abas para cada tipo de IPC
        self.pipe_tab = ttk.Frame(self.notebook)
        self.shm_tab = ttk.Frame(self.notebook)
        self.socket_tab = ttk.Frame(self.notebook)
        
        self.notebook.add(self.pipe_tab, text='Pipes')
        self.notebook.add(self.shm_tab, text='Shared Memory')
        self.notebook.add(self.socket_tab, text='Sockets')
        
        # Conteúdo de cada aba
        self._create_pipe_tab()
        self._create_shm_tab()
        self._create_socket_tab()

    def _update_log(self, log_area, data):
        """Atualiza uma área de log com dados formatados do backend."""
        
        def _append_log(message):
            log_area.config(state='normal')
            log_area.insert(tk.END, message + "\n")
            log_area.config(state='disabled')
            log_area.see(tk.END)

        timestamp = time.strftime('%H:%M:%S')
        
        pid = data.get("pid")
        pid_str = f" (PID: {pid})" if pid else ""
        
        log_message = f"[{timestamp}]{pid_str} "
        
        # Customização para o módulo de pipes para manter a formatação original
        if data.get("module") == "pipes" and data.get("type") == "data":
            source_map = {
                "pai -> filho": "PAI -> FILHO",
                "filho -> pai (eco)": "FILHO -> PAI (ECO)"
            }
            source = source_map.get(data.get("source"), data.get("source", "N/A"))
            log_message += f"{source}: {data.get('data', '')}"
        elif data.get("type") == "status":
            log_message += f"STATUS: {data.get('message', '')}"
        elif data.get("type") == "data":
            log_message += f"DADO: \"{data.get('data', '')}\" (de {data.get('source', 'N/A')})"
        elif data.get("type") == "error":
            log_message += f"ERRO: {data.get('error', 'Erro desconhecido')}"
        else:
            log_message += f"RAW: {data}"
            
        self.notebook.after(0, _append_log, log_message)

    def _create_pipe_tab(self):
        """Cria a aba de Pipes"""
        frame = self.pipe_tab
        
        entry_label = ttk.Label(frame, text="Mensagem:")
        entry_label.pack(pady=5)
        
        self.pipe_message_entry = ttk.Entry(frame, width=50)
        self.pipe_message_entry.pack(pady=5)
        
        send_button = ttk.Button(
            frame, 
            text="Comunicar via Pipe",
            command=self._send_pipe_message
        )
        send_button.pack(pady=10)
        
        self.pipe_log_area = scrolledtext.ScrolledText(frame, wrap=tk.WORD, height=15, width=80)
        self.pipe_log_area.pack(pady=10, padx=10)
        self.pipe_log_area.config(state='disabled')

    def _send_pipe_message(self):
        """Inicia a comunicação via pipes"""
        message = self.pipe_message_entry.get() or "Default Pipe Message"
        self.pipe_log_area.config(state='normal')
        self.pipe_log_area.delete(1.0, tk.END)
        
        self.backend_manager.start_process(
            module="pipes",
            executable="pipe_demo",
            args=[message],
            callback=lambda data: self._update_log(self.pipe_log_area, data)
        )

    def _create_shm_tab(self):
        """Cria a aba de Memória Compartilhada"""
        frame = self.shm_tab
        
        # Entrada de mensagem
        entry_label = ttk.Label(frame, text="Mensagem:")
        entry_label.pack(pady=5)
        
        self.shm_message_entry = ttk.Entry(frame, width=50)
        self.shm_message_entry.pack(pady=5)
        
        # Botão para enviar
        send_button = ttk.Button(
            frame, 
            text="Comunicar via Shared Memory",
            command=self._send_shm_message
        )
        send_button.pack(pady=10)
        
        # Área de log
        self.shm_log_area = scrolledtext.ScrolledText(frame, wrap=tk.WORD, height=15, width=80)
        self.shm_log_area.pack(pady=10, padx=10)
        self.shm_log_area.config(state='disabled')

    def _send_shm_message(self):
        """Inicia a comunicação via memória compartilhada"""
        message = self.shm_message_entry.get() or "Default SHM Message"
        self.shm_log_area.config(state='normal')
        self.shm_log_area.delete(1.0, tk.END)
        
        self.backend_manager.start_process(
            module="shm",
            executable="shm_demo",
            args=[message],
            callback=lambda data: self._update_log(self.shm_log_area, data)
        )

    def _create_socket_tab(self):
        """Cria a aba de Sockets (ainda não implementado)"""
        frame = self.socket_tab
        label = ttk.Label(frame, text="A funcionalidade de Sockets ainda não foi implementada.")
        label.pack(pady=20)
