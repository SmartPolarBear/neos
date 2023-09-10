cmake_minimum_required(VERSION 3.20)

add_executable(neldr head64.S entry64.c draw.c terminal.c elf.c fs.c ext2.c mem.c utils.c)

set_property(SOURCE loaderhead.S PROPERTY LANGUAGE C)

target_include_directories(neldr PRIVATE include)


target_compile_options(neldr BEFORE
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
        PRIVATE -nostdinc
)

target_link_options(neldr BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -fno-pie
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/loader64.ld)

add_custom_command(TARGET neldr POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:neldr> > ${CMAKE_CURRENT_BINARY_DIR}/neldr.asm
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:neldr> ${CMAKE_CURRENT_BINARY_DIR}/neldrbin
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/neldr.asm ${CMAKE_CURRENT_BINARY_DIR}/neldrbin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)