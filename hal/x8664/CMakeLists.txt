cmake_minimum_required(VERSION 3.20)

cmake_minimum_required(VERSION 3.20)

add_executable(halx8664
        halmain.c
        ops.c
)


target_include_directories(halx8664 PRIVATE include)

target_compile_options(halx8664 BEFORE
        PRIVATE -ffreestanding
        PRIVATE -fno-pie
        PRIVATE -fno-pic
        PRIVATE  -fno-common
        PRIVATE -fno-omit-frame-pointer
        PRIVATE -fno-strict-aliasing
        PRIVATE -m64
        PRIVATE -MD
        PRIVATE -O0
        PRIVATE -mcmodel=large
        PRIVATE -mtls-direct-seg-refs
        PRIVATE -mno-red-zone
        PRIVATE -nostdlib
        PRIVATE -nodefaultlibs
        PRIVATE -static
        PRIVATE -shared
)

target_link_options(halx8664 BEFORE
        PRIVATE -nostdlib
        PRIVATE -fno-pic
        PRIVATE -fno-pie
        PRIVATE -static
        PRIVATE -r
        PRIVATE -Bstatic
        PRIVATE -Bsymbolic
        PRIVATE -Wl,-r
        PRIVATE -Wl,-Bstatic
        PRIVATE -Wl,-Bsymbolic
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/hal.ld)

add_custom_command(TARGET halx8664 POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:halx8664> > ${CMAKE_CURRENT_BINARY_DIR}/halx8664.asm
        COMMAND readelf -a $<TARGET_FILE:halx8664> > ${CMAKE_CURRENT_BINARY_DIR}/halx8664.txt
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/halx8664.asm ${CMAKE_CURRENT_BINARY_DIR}/halx8664.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)