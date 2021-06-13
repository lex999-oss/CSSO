cmake_minimum_required(VERSION 3.17)
project(tema7_8 C)

set(CMAKE_C_STANDARD 99)

add_executable(tema7_8 CraciunAlexandru3CSSO2.c)

target_link_libraries(tema7_8 -lwininet)