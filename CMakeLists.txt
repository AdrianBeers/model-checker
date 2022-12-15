cmake_minimum_required(VERSION 3.23)
project(model_checker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s -O3")
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(model_checker main.cpp Parser.cpp Parser.h Formulae.cpp Formulae.h LTS.cpp LTS.h Algorithm.cpp Algorithm.h)
