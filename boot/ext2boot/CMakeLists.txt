cmake_minimum_required(VERSION 3.15)

add_executable(ext2bootsec bootsec.S utils.S)

set_property(SOURCE bootsec.S PROPERTY LANGUAGE C)
set_property(SOURCE utils.S PROPERTY LANGUAGE C)

target_include_directories(ext2bootsec PRIVATE include)

target_compile_options(ext2bootsec BEFORE
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

target_link_options(ext2bootsec BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -fno-pie
        PRIVATE -O0
        PRIVATE -m32
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/bootsec.ld)

add_custom_command(TARGET ext2bootsec POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:ext2bootsec> > ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsec.asm
        COMMAND readelf -a $<TARGET_FILE:ext2bootsec> > ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsec.dump
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:ext2bootsec> ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsecbin
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsec.asm ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsecbin ${CMAKE_CURRENT_BINARY_DIR}/ext2bootsec.dump
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
