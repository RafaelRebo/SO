cmake_minimum_required(VERSION 3.26)
project(P0 C)

set(CMAKE_C_STANDARD 99)

add_executable(P0 p0.c
        lista.c
        types.h
        lista.h
        listaficheros.c
        listaficheros.h
        aux.c)
