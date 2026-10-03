cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto y lenguaje utilizado
project(SubsetConstruction C)

# Especificar el estándar de C a utilizar (C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Configurar banderas de compilación para reportar advertencias importantes
if(MSVC)
    # Banderas para el compilador de Visual Studio
    add_compile_options(/W4)
else()
    # Banderas para GCC o Clang (Linux/macOS)
    add_compile_options(-Wall -Wextra -O2)
endif()

# Definir el ejecutable y asociarlo con el archivo de código fuente
add_executable(subset_construction main.c)
