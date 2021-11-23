cmake_minimum_required(VERSION 3.16)
project(server C)

find_package(Threads REQUIRED)
set(CMAKE_C_STANDARD 99)
SET(CMAKE_C_FLAGS -pthread)

add_executable(server main.c game.h stateMachine.h)