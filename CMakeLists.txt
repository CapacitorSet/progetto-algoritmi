cmake_minimum_required(VERSION 3.14)
project(progetto_algoritmi)

set(CMAKE_CXX_STANDARD 17)

add_executable(progetto_algoritmi
        src/main.cpp
        src/parser.cpp
        src/parser.h
        src/ast.h
        src/ast.cpp
        src/generic.h
        src/circuit.hpp
        src/expression.hpp
        src/truthvalue.h
        src/truthvalue.cpp
        src/simulation.h
        src/simulation.cpp
        src/analysis.h
        src/analysis.cpp
        src/utils.h)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wimplicit-fallthrough -fsanitize=address -g -ferror-limit=1")
set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=address -g")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wimplicit-fallthrough -g -ferror-limit=1")
#set(CMAKE_EXE_LINKER_FLAGS "-g")