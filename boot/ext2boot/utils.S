.code16
.globl PutStr
PutStr:
    # Input:
    #   str: pointer to the 0-terminated string to display (passed in %esi)

    # Save registers
    pusha

    # print string in esi
PrintLoop:
    lodsb
    test %al, %al
    jz ExitPrint
    mov $0x0E, %ah
    int $0x10
    jmp PrintLoop
ExitPrint:
    # print newline
    mov $0x0E, %ah
    mov $0x0A, %al
    int $0x10

    mov $0x0E, %ah
    mov $0x0D, %al
    int $0x10

    # Restore registers
    popa

    # Return
    ret

.globl MemCmp
// same as memcmp in C
MemCmp:
    push %ebp
    mov %esp, %ebp
    push %ecx
    push %esi
    push %edi

    mov 6(%ebp), %di
    mov 8(%ebp), %si
    mov 10(%ebp), %cx
    hlt

    cld
    repe cmpsb
    jne NotEqu
Equ:
    xor %eax, %eax
    jmp End
NotEqu:
    mov %esi, %eax
    sub %edi, %eax
End:
    pop %edi
    pop %esi
    pop %ecx
    pop %ebp
    ret