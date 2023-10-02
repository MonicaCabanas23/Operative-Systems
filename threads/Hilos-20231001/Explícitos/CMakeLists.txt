cmake_minimum_required(VERSION 3.17)
project(Hilos)

set(CMAKE_CXX_STANDARD 17)

add_executable(Hilos main.cpp explicitos.cpp explicitos.h)
target_link_libraries(Hilos pthread)