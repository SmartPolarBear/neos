cmake_minimum_required(VERSION 3.15)

add_library(LAI STATIC
        "lai/core/error.c"
        "lai/core/eval.c"
        "lai/core/exec.c"
        "lai/core/exec-operand.c"
        "lai/core/libc.c"
        "lai/core/ns.c"
        "lai/core/object.c"
        "lai/core/opregion.c"
        "lai/core/os_methods.c"
        "lai/core/variable.c"
        "lai/core/vsnprintf.c"
        "lai/helpers/pc-bios.c"
        "lai/helpers/pci.c"
        "lai/helpers/resource.c"
        "lai/helpers/sci.c"
        "lai/helpers/pm.c"
        "lai/drivers/ec.c"
        "lai/drivers/timer.c")

target_include_directories(LAI PUBLIC "lai/include")

target_compile_options(LAI BEFORE
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
