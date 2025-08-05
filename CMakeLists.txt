# Define a versão mínima do CMake necessária para o projeto
cmake_minimum_required(VERSION 3.10)

# Define o nome do projeto
project(Desafio_FPF_Tech)

# Permite que todas as bibliotecas sejam compiladas com código independente de posição (necessário para bibliotecas compartilhadas)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# =====================
# Biblioteca gentexts
# =====================
# Cria a biblioteca compartilhada gentexts a partir do código C
add_library(gentexts SHARED lib/lib_gentexts.c)
target_include_directories(gentexts PUBLIC lib)

# =====================
# Backend
# =====================
# Cria a biblioteca estática backend_lib a partir do código C++ do backend
add_library(backend_lib STATIC backend/backend.cpp)
target_include_directories(backend_lib PUBLIC backend lib)
target_link_libraries(backend_lib PUBLIC gentexts curl)

# =====================
# Frontend (Qt)
# =====================
# Busca os componentes necessários do Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Multimedia Concurrent)
# Ativa geração automática de arquivos moc (necessário para Qt signals/slots)
set(CMAKE_AUTOMOC ON)
add_executable(frontend frontend/main.cpp frontend/gui.cpp)
target_include_directories(frontend PUBLIC frontend backend lib)
target_link_libraries(frontend PRIVATE backend_lib Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Concurrent curl)