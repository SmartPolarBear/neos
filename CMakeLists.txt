cmake_minimum_required(VERSION 3.20)

project(neos C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# common public headers
include_directories(include)

add_subdirectory(libs)

add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(hal)

add_custom_target(image
    COMMAND sh "${CMAKE_SOURCE_DIR}/scripts/mkdisk.sh"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
    DEPENDS bootsector osldr neldr neosknl halx8664)