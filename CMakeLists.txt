cmake_minimum_required(VERSION 3.13)
project(lssh C)

include_directories(/usr/include/readline)
set(CMAKE_C_STANDARD 11)

add_executable(lssh main.c)
