cmake_minimum_required(VERSION 3.10)
project(projeto-ipc C)

# Variável para todos os arquivos fonte do backend
file(GLOB BACKEND_SOURCES
    src/backend/pipes/*.c
    src/backend/sockets/*.c
    src/backend/shared_memory/*.c
    src/backend/common/*.c
    src/backend/main.c
)

# Adiciona o executável
add_executable(ipc_backend ${BACKEND_SOURCES})

# Se precisar de threads, pode descomentar
# find_package(Threads REQUIRED)
# target_link_libraries(ipc_backend PRIVATE Threads::Threads)
