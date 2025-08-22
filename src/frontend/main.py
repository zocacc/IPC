# main.py
#!/usr/bin/env python3
"""
IPC Demo - Demonstração de Comunicação Entre Processos
Autor: [Seu Nome]
"""

import sys
import os
from gui.main_window import IPCDemoWindow

def check_backend_executables():
    """Verifica se os executáveis do backend existem"""
    build_dir = "./build"
    required_executables = ["pipe_demo", "socket_demo", "shm_demo"]
    
    if not os.path.exists(build_dir):
        print("Erro: Diretório 'build' não encontrado.")
        print("Execute: mkdir build && cd build && cmake .. && make")
        return False
    
    missing = []
    for exe in required_executables:
        if not os.path.exists(os.path.join(build_dir, exe)):
            missing.append(exe)
    
    if missing:
        print(f"Erro: Executáveis não encontrados: {', '.join(missing)}")
        print("Execute: cd build && make")
        return False
    
    return True

def main():
    """Função principal"""
    print("IPC Demo - Iniciando aplicação...")
    
    # Verificar dependências
    if not check_backend_executables():
        sys.exit(1)
    
    # Iniciar interface
    try:
        app = IPCDemoWindow()
        app.run()
    except KeyboardInterrupt:
        print("\\nAplicação interrompida pelo usuário.")
    except Exception as e:
        print(f"Erro inesperado: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()