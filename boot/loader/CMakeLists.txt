cmake_minimum_required(VERSION 3.15)

add_executable(osldr loader.c loaderhead.S vesa.c vgafont.c)

set_property(SOURCE loaderhead.S PROPERTY LANGUAGE C)

target_include_directories(osldr PRIVATE include)

target_compile_options(osldr BEFORE
        PRIVATE -fno-pic
        PRIVATE -m32
        PRIVATE -Os
        PRIVATE -nostdinc)

target_link_options(osldr BEFORE
        PRIVATE -Wl,-N
        PRIVATE -m32
        PRIVATE -nostdlib
        PRIVATE -static
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/loader.ld)

add_custom_command(TARGET osldr POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:osldr> > ${CMAKE_CURRENT_BINARY_DIR}/osldr.asm
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:osldr> ${CMAKE_CURRENT_BINARY_DIR}/osldrbin
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/osldr.asm ${CMAKE_CURRENT_BINARY_DIR}/osldrbin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
