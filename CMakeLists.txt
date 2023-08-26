cmake_minimum_required(VERSION 3.20)

project(neos C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_subdirectory(boot)
