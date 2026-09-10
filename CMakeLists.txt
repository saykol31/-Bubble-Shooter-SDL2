cmake_minimum_required(VERSION 3.15)

project(BubbleShooter
    VERSION 1.0
    DESCRIPTION "Bubble Shooter - SDL2"
    LANGUAGES CXX
)

# ============================================================
# CONFIGURACIÓN DEL COMPILADOR
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# DIRECTORIOS DE SALIDA
# ============================================================

# Ejecutables:
#   bin/BubbleShooter
#
# Esto mantiene la estructura:
#
# Bubble Shooter/
# ├── bin/
# │   └── BubbleShooter
# ├── build/
# ├── main.cpp
# └── CMakeLists.txt
#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin
)

# ============================================================
# FUENTES
# ============================================================

set(SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
)

# ============================================================
# DEPENDENCIAS SDL2
# ============================================================

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# ============================================================
# EJECUTABLE
# ============================================================

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

# ============================================================
# DIRECTORIOS DE INCLUSIÓN
# ============================================================

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${SDL2_INCLUDE_DIRS}
)

# ============================================================
# LIBRERÍAS
# ============================================================

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDL2_LIBRARIES}
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_mixer::SDL2_mixer
)

# ============================================================
# OPCIONES DEL COMPILADOR
# ============================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -O2
    )

endif()

# ============================================================
# INFORMACIÓN DEL PROYECTO
# ============================================================

message(STATUS "")
message(STATUS "==========================================")
message(STATUS " Bubble Shooter")
message(STATUS "==========================================")
message(STATUS " Version:       ${PROJECT_VERSION}")
message(STATUS " C++:           ${CMAKE_CXX_STANDARD}")
message(STATUS " Compiler:      ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS " Build type:    ${CMAKE_BUILD_TYPE}")
message(STATUS " SDL2:          encontrado")
message(STATUS " SDL2_image:    encontrado")
message(STATUS " SDL2_ttf:      encontrado")
message(STATUS " SDL2_mixer:    encontrado")
message(STATUS " Output:        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "==========================================")
message(STATUS "")
