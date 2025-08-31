#!/usr/bin/env python3
"""
Teste simples de integração para verificar se os programas C estão gerando JSON válido
Testa: pipes, shared memory e sockets
"""

import subprocess
import json
import sys
import os

def test_json_output(program_path, args, description):
    """Testa se um programa gera JSON válido"""
    print(f"\n=== Testando {description} ===")
    
    try:
        # Executa o programa
        result = subprocess.run([program_path] + args, 
                              capture_output=True, 
                              text=True, 
                              timeout=10)
        
        if result.returncode != 0:
            print(f"❌ Programa falhou com código {result.returncode}")
            if result.stderr:
                print(f"Erro: {result.stderr}")
            return False
        
        # Verifica se há saída
        if not result.stdout.strip():
            print("❌ Nenhuma saída gerada")
            return False
        
        # Conta linhas de JSON
        lines = result.stdout.strip().split('\n')
        json_lines = [line for line in lines if line.strip()]
        
        print(f"📊 Gerou {len(json_lines)} linhas de saída")
        
        # Tenta fazer parse de cada linha como JSON
        valid_json_count = 0
        for i, line in enumerate(lines):
            if line.strip():
                try:
                    json_obj = json.loads(line)
                    valid_json_count += 1
                    
                    # Verifica se tem os campos esperados
                    if 'type' in json_obj and 'timestamp' in json_obj:
                        print(f"✅ Linha {i+1}: JSON válido - tipo: {json_obj['type']}")
                    else:
                        print(f"⚠️  Linha {i+1}: JSON válido mas campos inesperados")
                        
                except json.JSONDecodeError as e:
                    print(f"❌ Linha {i+1}: JSON inválido - {e}")
                    print(f"   Conteúdo: {line[:100]}...")
        
        print(f"📈 {valid_json_count}/{len(json_lines)} linhas são JSON válido")
        
        if valid_json_count > 0:
            print(f"✅ {description} funcionando - gera JSON válido")
            return True
        else:
            print(f"❌ {description} falhou - nenhum JSON válido gerado")
            return False
            
    except subprocess.TimeoutExpired:
        print(f"❌ {description} falhou - timeout após 10 segundos")
        return False
    except FileNotFoundError:
        print(f"❌ {description} falhou - programa não encontrado em {program_path}")
        return False
    except Exception as e:
        print(f"❌ {description} falhou com erro: {e}")
        return False

def test_json_escaping():
    """Testa especificamente o escape de caracteres especiais no JSON"""
    print(f"\n=== Testando Escape de JSON ===")
    
    project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    build_dir = os.path.join(project_root, "build")
    
    # Testa com mensagem que contém caracteres especiais
    test_message = 'teste com "aspas" e \\backslashes\\ e\\nquebras\\nde linha'
    
    try:
        result = subprocess.run([os.path.join(build_dir, "shm_demo"), test_message], 
                              capture_output=True, 
                              text=True, 
                              timeout=10)
        
        if result.returncode != 0:
            print("❌ Teste de escape falhou - programa não executou")
            return False
        
        # Procura pela linha que contém a mensagem de teste
        lines = result.stdout.strip().split('\n')
        test_lines = []
        
        for line in lines:
            if 'teste com' in line and 'aspas' in line:
                test_lines.append(line)
        
        if not test_lines:
            print("❌ Teste de escape falhou - linhas de teste não encontradas")
            print("   Procurando por mensagens que contenham 'teste com' e 'aspas'")
            return False
        
        print(f"📊 Encontradas {len(test_lines)} linhas com a mensagem de teste")
        
        # Verifica se todas as linhas são JSON válido
        valid_count = 0
        for i, test_line in enumerate(test_lines):
            try:
                json_obj = json.loads(test_line)
                valid_count += 1
                print(f"✅ Linha {i+1}: JSON válido com caracteres especiais")
                
                # Verifica se a mensagem foi escapada corretamente
                if 'message' in json_obj:
                    message = json_obj['message']
                    if '\\"' in message:
                        print(f"   ✅ Aspas duplas escapadas corretamente")
                    if '\\\\' in message:
                        print(f"   ✅ Backslashes escapados corretamente")
                    if '\\n' in message:
                        print(f"   ✅ Quebras de linha escapadas corretamente")
                        
            except json.JSONDecodeError as e:
                print(f"❌ Linha {i+1}: JSON inválido - {e}")
                print(f"   Conteúdo: {test_line[:100]}...")
        
        print(f"📈 {valid_count}/{len(test_lines)} linhas são JSON válido")
        
        if valid_count == len(test_lines):
            print("✅ Todos os JSONs com caracteres especiais são válidos")
            return True
        else:
            print("⚠️  Alguns JSONs com caracteres especiais são inválidos")
            return False
            
    except Exception as e:
        print(f"❌ Teste de escape falhou com erro: {e}")
        return False

def main():
    """Função principal do teste"""
    print("🧪 Teste de Integração - Verificação de JSON Output")
    print("=" * 50)
    
    # Diretório base do projeto
    project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    
    # Caminhos para os executáveis (assumindo que foram compilados)
    build_dir = os.path.join(project_root, "build")
    
    # Lista de testes
    tests = [
        {
            "path": os.path.join(build_dir, "pipe_demo"),
            "args": ["teste_pipes"],
            "description": "Pipes Demo"
        },
        {
            "path": os.path.join(build_dir, "shm_demo"),
            "args": ["teste_shm"],
            "description": "Shared Memory Demo"
        },
        {
            "path": os.path.join(build_dir, "socket_demo"),
            "args": ["teste_sockets"],
            "description": "Sockets Demo"
        }
    ]
    
    # Executa os testes
    results = []
    for test in tests:
        success = test_json_output(test["path"], test["args"], test["description"])
        results.append((test["description"], success))
    
    # Executa o teste de escape
    escape_success = test_json_escaping()
    results.append(("JSON Escaping", escape_success))
    
    # Resumo final
    print("\n" + "=" * 50)
    print("📋 RESUMO DOS TESTES")
    print("=" * 50)
    
    passed = 0
    total = len(results)
    
    for description, success in results:
        status = "✅ PASSOU" if success else "❌ FALHOU"
        print(f"{description}: {status}")
        if success:
            passed += 1
    
    print(f"\n📊 Resultado: {passed}/{total} testes passaram")
    
    if passed == total:
        print("🎉 Todos os testes passaram! JSON output funcionando corretamente.")
        return 0
    else:
        print("⚠️  Alguns testes falharam. Verifique os logs acima.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
