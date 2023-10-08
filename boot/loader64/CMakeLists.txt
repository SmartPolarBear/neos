cmake_minimum_required(VERSION 3.20)

add_executable(neldr
        head64.S
        entry64.c
        draw.c
        terminal.c
        fs.c
        disk.c
        ext2.c
        mem.c
        utils.c
        pci.c
        acpi.c
        elf.c
        loader.c
        log.c
        $<TARGET_OBJECTS:LAI>
)

set_property(SOURCE loaderhead.S PROPERTY LANGUAGE C)

add_subdirectory(acpi)

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
)

target_link_options(neldr BEFORE
        PRIVATE -Wl,-N
        PRIVATE -Wl,-z nodefaultlib
        PRIVATE -fno-pie
        PRIVATE -static
        PRIVATE -nostdlib
        PRIVATE -Wl,--build-id=none
        PRIVATE -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/loader64.ld)

target_link_libraries(neldr PRIVATE LAI)

add_dependencies(neldr loader16)

# create flat binary and combine with loader16 for neldr file
# use dd:
#   1) the loader16bin will be the first 0x1000 bytes of neldr
#   2) the neldrbin, starting at 0x1000, will be the rest of neldr
add_custom_command(TARGET neldr POST_BUILD
        COMMAND objdump -S $<TARGET_FILE:neldr> > ${CMAKE_CURRENT_BINARY_DIR}/neldr.asm
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:neldr> ${CMAKE_CURRENT_BINARY_DIR}/neldrbin
        COMMAND objcopy -S -O binary -j .text $<TARGET_FILE:loader16> ${CMAKE_CURRENT_BINARY_DIR}/loader16bin
        COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/loader16bin of=${CMAKE_CURRENT_BINARY_DIR}/neldr bs=1 count=4096 conv=notrunc
        COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/neldrbin of=${CMAKE_CURRENT_BINARY_DIR}/neldr bs=1 seek=4096 conv=notrunc
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/neldr.asm ${CMAKE_CURRENT_BINARY_DIR}/neldrbin ${CMAKE_CURRENT_BINARY_DIR}/loader16bin ${CMAKE_CURRENT_BINARY_DIR}/neldr
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

