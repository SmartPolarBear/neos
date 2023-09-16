cmake_minimum_required(VERSION 3.20)

cmake_minimum_required(VERSION 3.20)

add_executable(halx8664
        halmain.c
)

target_include_directories(halx8664 PRIVATE include)

target_compile_options(halx8664 BEFORE
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

target_link_options(halx8664 BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -Wl,-q
#        PRIVATE -Wl,-r
        PRIVATE -fno-pie
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/hal.ld)

add_custom_command(TARGET halx8664 POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:halx8664> > ${CMAKE_CURRENT_BINARY_DIR}/halx8664.asm
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/halx8664.asm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)