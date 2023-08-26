.code16
.globl OSLoaderEntry
OSLoaderEntry:
    // print a message "..."
    mov $0x03, %si
EnterMsg:
    mov $0x0e, %ah
    mov $'.', %al
    int $0x10
    dec %si
    jnz EnterMsg

Spin:
    hlt
    jmp Spin