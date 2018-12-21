cmake_minimum_required(VERSION 3.13)
project(lssh C)
set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(/usr/include/readline)
set(CMAKE_C_STANDARD 11)

add_executable(lssh main.c)
target_link_libraries(srepl readline)
