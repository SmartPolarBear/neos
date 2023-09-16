cmake_minimum_required(VERSION 3.20)

add_executable(neosknl
        kernhead.S
        kernmain.c
)

#set_property(SOURCE kernhead.S PROPERTY LANGUAGE C)

target_include_directories(neosknl PRIVATE include)

target_compile_options(neosknl BEFORE
        PRIVATE -ffreestanding
        PRIVATE -fno-pie
        PRIVATE -m64
        PRIVATE -MD
        PRIVATE  -fno-common
        PRIVATE -mcmodel=large
        PRIVATE -fno-omit-frame-pointer
        PRIVATE -mtls-direct-seg-refs
        PRIVATE -mno-red-zone
        PRIVATE -fno-pic
        PRIVATE -O0
        PRIVATE -static
        PRIVATE -fno-strict-aliasing
        PRIVATE -nostdlib
        PRIVATE -nodefaultlibs
)

target_link_options(neosknl BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -fno-pie
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld)

add_custom_command(TARGET neosknl POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:neosknl> > ${CMAKE_CURRENT_BINARY_DIR}/neosknl.asm
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/neosknl.asm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)