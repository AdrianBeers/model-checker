cmake_minimum_required(VERSION 3.23)
project(model_checker)

set(CMAKE_CXX_STANDARD 20)

add_executable(model_checker main.cpp Parser.cpp Parser.h Formulae.cpp Formulae.h LTS.cpp LTS.h)
