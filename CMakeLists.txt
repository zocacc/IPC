cmake_minimum_required(VERSION 3.10)
project(IPCDemo)

enable_testing()

set(CMAKE_C_STANDARD 99)

# Diretórios
set(BACKEND_DIR src/backend)
set(COMMON_DIR ${BACKEND_DIR}/common)

# Arquivos comuns
set(COMMON_SOURCES
    ${COMMON_DIR}/json_output.c
)

# Executáveis para cada módulo IPC
add_executable(pipe_demo 
    ${BACKEND_DIR}/pipes/pipe_demo.c
    ${COMMON_SOURCES}
)

add_executable(socket_demo 
    ${BACKEND_DIR}/sockets/socket_demo.c
    ${COMMON_SOURCES}
)

add_executable(shm_demo 
    ${BACKEND_DIR}/shared_memory/shm_demo.c
    ${BACKEND_DIR}/shared_memory/shm_handler.c
    ${COMMON_SOURCES}
)

# Diretório de includes
target_include_directories(pipe_demo PRIVATE ${COMMON_DIR} ${BACKEND_DIR}/pipes)
target_include_directories(socket_demo PRIVATE ${COMMON_DIR} ${BACKEND_DIR}/sockets)
target_include_directories(shm_demo PRIVATE ${COMMON_DIR} ${BACKEND_DIR}/shared_memory)

# Bibliotecas do sistema (se necessárias)
target_link_libraries(shm_demo rt)  # Para shared memory no Linux

# ==============
# Testes
# ==============
# Teste para shared memory
add_executable(shm_test 
    tests/backend_tests/test_shm.c
    ${BACKEND_DIR}/shared_memory/shm_handler.c
    ${COMMON_SOURCES}
)
target_include_directories(shm_test PRIVATE ${COMMON_DIR} ${BACKEND_DIR}/shared_memory)
target_link_libraries(shm_test rt)
add_test(NAME shm_test COMMAND shm_test)

# Teste para json_output
add_executable(json_output_test
    tests/backend_tests/test_json_output.c
    ${COMMON_SOURCES}
)
target_include_directories(json_output_test PRIVATE ${COMMON_DIR})
add_test(NAME json_output_test COMMAND json_output_test)

# Teste para pipe
add_executable(pipe_test
    tests/backend_tests/pipe_test.c
    ${COMMON_SOURCES}
)
target_include_directories(pipe_test PRIVATE ${COMMON_DIR} ${CMAKE_SOURCE_DIR})
add_test(NAME pipe_test COMMAND pipe_test)
