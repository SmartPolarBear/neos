#include "defs.h"

.section .text

// This bootsector parse the ext2 fuckers and find neldr file:
// 1) load first 1024bit of neldr to 0x2000. 
//    It is 16bit code which will fuck with things must done in real mode with BIOS
//    It will finally switch to protected mode and call the rest of neldr
// 2) the rest of neldr is load to HIGHMEM(1MB+64KB), which do other fucking good things and enter long mode
.code16
.globl OSLoaderEntry
OSLoaderEntry:
    cli

    // pop bootable partition item address
    pop %di 

    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    // make BIOS happy: we are 32bit/64bit hybrid kernel
    mov $0xec00, %ax
    mov $0x3, %bl
    int $0x15

    // print a message "..."
    lea EnterMsgStr, %si
    call PutStr

    // Physical address line A20 is tied to zero so that the first PCs
    // with 2 MB would run software that assumed 1 MB.  Undo that.
SetA20.1:
    inb     $0x64,%al               # Wait for not busy
    testb   $0x2,%al
    jnz     SetA20.1

    movb    $0xd1,%al               # 0xd1 -> port 0x64
    outb    %al,$0x64

SetA20.2:
    inb     $0x64,%al               # Wait for not busy
    testb   $0x2,%al
    jnz     SetA20.2

    movb    $0xdf,%al               # 0xdf -> port 0x60
    outb    %al,$0x60

    // load the ext2 superblock
    mov $0x7c0, %ax
    mov %ax, %es
    movw %es:8(%di), %ax // low 16bit of relative sector
    movw %es:10(%di), %dx // high 16bit of relative sector

    add $0x2, %ax   // 2 sector is the offset of superblock in the first sector
    adc $0x0, %dx   // add carry bit

    mov $0x2, %cx   // 2 is the size of superblock (measured in sectors)
    lea Ext2BootEnd, %bx // put superblock right after this loader

    call ReadLBA

    cmp $0, %ax
    jne FailRead

    


Spin:
    hlt
    jmp Spin
FailRead:
    lea ReadFailStr, %si
    call PutStr
    jmp Spin


.globl ReadLBA
ReadLBA:
    // read using LBA in extened mode
    // Input:   %ax = low 16bit of relative sector
    //          %dx = high 16bit of relative sector
    //          %cx = number of sector
    //          %bx = address of buffer
    pusha
    
    // fill a disk address packet
    lea DiskAddressPacket, %si
    movw %cx, 2(%si) // number of sector
    movw %bx, 4(%si) // offset of buffer
    movw %ax, 8(%si) // low 16bit of relative sector
    movw %dx, 10(%si) // high 16bit of relative sector

    xor %dx, %dx
    movw %dx, 6(%si) // segment of buffer is always 0
    movw %dx, 12(%si)
    movw %dx, 14(%si) // upper 16bit of 48bit LBA is always 0

    movb $0x42, %ah // read using LBA in extened mode
    movb $0x80, %dl // drive number
    int $0x13
    jc ErrorReadLBA
    cmp $0x0, %ah // double check success
    jne ErrorReadLBA

    popa
    mov $0, %ax // return 0 if success
    ret
ErrorReadLBA:
    popa
    mov $1, %ax
    ret


.section .data
    .align 4

// string "..."
.globl EnterMsgStr
EnterMsgStr:
    .ascii "..."
    .byte 0

ReadFailStr:
    .ascii "Disk error."
    .byte 0

DiskAddressPacket:
    .byte 0x10  // always 16
    .byte 0     // reserved, always 0
    .word 0     // number of sector
    .word 0
    .word 0     // offset of buffer
    .word 0
    .word 0     // low 32bit of LBA
    .word 0
    .word 0     // high 16bit of LBA
