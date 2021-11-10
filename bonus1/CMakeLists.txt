# cmake_minimum_required(VERSION <specify CMake version here>)
project(bonus1 C)

set(CMAKE_C_STANDARD 11)

add_executable(bonus1 echo_client.c reverse_client.c)