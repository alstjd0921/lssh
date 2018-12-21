cmake_minimum_required(VERSION 3.13)
project(lssh C)

include_directories(/usr/include/readline)
link_directories(/lib/x86_64-linux-gnu/)

set(CMAKE_C_STANDARD 11)

add_executable(lssh main.c)
target_link_libraries(lssh readline)
