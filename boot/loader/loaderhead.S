#include "defs.h"
#include "asm.h"
#include "mmu.h"

.code16
.globl OSLoaderEntry
OSLoaderEntry:
    fuck:jmp fuck
    cli

    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    // set boot stack
    mov 0x7c00, %sp

    // make BIOS happy: we are 32bit/64bit hybrid kernel
    mov $0xec00, %ax
    mov $0x3, %bl
    int $0x15

    // print a message "..."
    mov $0x03, %si
EnterMsg:
    mov $0x0e, %ah
    mov $'.', %al
    int $0x10
    dec %si
    jnz EnterMsg

    // new line
    mov $0x0e, %ah
    mov $0x0a, %al
    int $0x10
    mov $0x0e, %ah
    mov $0x0d, %al
    int $0x10

    // Save memory map at 0x8000
ProbeMem:
    movl $0, MMAP_ADDR
    xorl %ebx, %ebx
    movw $0x8004, %di
StartProbeMem:
    movl $0xE820, %eax
    movl $20, %ecx
    movl $0x534d4150, %edx
    int $0x15
    jnc ContProbeMem
    movw $12345, 0x8000
    jmp FinishProbeMem
ContProbeMem:
    addw $20, %di
    incl 0x8000
    cmpl $0, %ebx
    jnz StartProbeMem
FinishProbeMem:

    // Copy VGA Font
    push %ds
    push %es
    mov $0x1130, %ax
    mov $0x06, %bh
    int $0x10
    push %es
    pop %ds
    pop %es
    mov %bp, %si
    mov $VGA_FONT_ADDR, %di
    mov $0x400, %cx // length: 256*16/4
    cld
    rep movsd
    pop %ds

    // Initialize VESA
    mov $vbeInfo, %di
    mov $0x4f00, %ax
    int $0x10
    cmp $0x004f, %ax
    jne FailSpin
    call InitializeVesa

    // Switch to protected mode
    lgdt GDTDesc
    movl %cr0, %eax
    orl $CR0_PE, %eax
    movl %eax, %cr0
    ljmp  $(SEG_KCODE<<3), $OSLoaderEntry32

.code32
OSLoaderEntry32:
    movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
    movw    %ax, %ds                # -> DS: Data Segment
    movw    %ax, %es                # -> ES: Extra Segment
    movw    %ax, %ss                # -> SS: Stack Segment
    movw    $0, %ax                 # Zero segments not ready for use
    movw    %ax, %fs                # -> FS
    movw    %ax, %gs                # -> GS

    // avoid disturb from previous eax values
    mov $0xdeadbeef, %eax

    // call LoaderMain32               # Different from other loaders, this **MAY** return, so use call instead of jmp

    // check return value of LoaderMain32
    cmp $0, %eax
    jne FailSpin

    // jump to kernel
    mov $NELDR_LOAD_ADDR, %ecx
    jmp *%ecx
FailSpin:
    hlt
    jmp FailSpin

# Bootstrap GDT 32
.p2align 2                                # force 4 byte alignment
GDT:
    SEG_NULLASM                             # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
    SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

GDTDesc:
    .word   (GDTDesc - GDT - 1)             # sizeof(gdt) - 1
    .long   GDT                            # address gdt

