cmake_minimum_required(VERSION 3.20)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c jrb.c jval.c fields.c dllist.c)
