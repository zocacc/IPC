cmake_minimum_required(VERSION 3.10)
project(IPCDemo)

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
    ${COMMON_SOURCES}
)

# Diretório de includes
target_include_directories(pipe_demo PRIVATE ${COMMON_DIR})
target_include_directories(socket_demo PRIVATE ${COMMON_DIR})
target_include_directories(shm_demo PRIVATE ${COMMON_DIR})

# Bibliotecas do sistema (se necessárias)
target_link_libraries(shm_demo rt)  # Para shared memory no Linux