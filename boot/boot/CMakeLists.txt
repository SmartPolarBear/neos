cmake_minimum_required(VERSION 3.20)

add_custom_target(bootsector
        COMMAND nasm -f bin -o "${CMAKE_CURRENT_BINARY_DIR}/bootsector" boot.asm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/bootsector"
        SOURCES boot.asm)