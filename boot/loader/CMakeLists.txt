cmake_minimum_required(VERSION 3.15)

add_executable(loader16 loaderhead.S vesa.c utils.c)

set_property(SOURCE loaderhead.S PROPERTY LANGUAGE C)

target_include_directories(loader16 PRIVATE include)

target_compile_options(loader16 BEFORE
        PRIVATE -ffreestanding
        PRIVATE -fno-pie
        PRIVATE -m32
        PRIVATE -MD
        PRIVATE  -fno-common
        PRIVATE -fno-omit-frame-pointer
        PRIVATE -mtls-direct-seg-refs
        PRIVATE -mno-red-zone
        PRIVATE -fno-pic
        PRIVATE -mno-red-zone
        PRIVATE -O0
        PRIVATE -static
        PRIVATE -fno-strict-aliasing
        PRIVATE -nostdlib
        PRIVATE -nodefaultlibs
        PRIVATE -nostdinc
)

target_link_options(loader16 BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -fno-pie
        PRIVATE -O0
        PRIVATE -m32
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/loader.ld)


add_custom_command(TARGET loader16 POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:loader16> > ${CMAKE_CURRENT_BINARY_DIR}/loader16.asm
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:loader16> ${CMAKE_CURRENT_BINARY_DIR}/loader16bin
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/loader16.asm ${CMAKE_CURRENT_BINARY_DIR}/loader16bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
