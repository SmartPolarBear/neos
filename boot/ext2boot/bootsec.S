#include "defs.h"

.section .text

// This bootsector parse the ext2 fuckers and find neldr file:
// 1) load first 1024bit of neldr to 0x2000. 
//    It is 16bit code which will fuck with things must done in real mode with BIOS
//    It will finally switch to protected mode and call the rest of neldr
// 2) the rest of neldr is load to HIGHMEM(1MB+64KB), which do other fucking good things and enter long mode
.code16
.globl Ext2BootEntry
Ext2BootEntry:
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

    mov $0x7c0, %ax
    mov %ax, %es
    movl %es:8(%di), %eax // low 32bit of relative sector
    add $0x2, %eax // skip the first 2 sectors
    movl %eax, StartLBA // save the start LBA of ext2 partition

    mov $0x2, %cx   // 2 is the size of superblock (measured in sectors)
    lea Ext2BootEnd, %bp // put superblock right after this loader

    // load the ext2 superblock
    push %cx
    push %bp
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments
    cmp $0, %ax
    jne FailRead

    // calculate block size of ext2 partition
    movl 24(%bp), %ecx // block size
    movl $0x400, %eax // 1024
    shl %ecx, %eax
    movl %eax, BlockSize

    // read group descriptor table
    mov $0x2, %cx // 2 is the size of group descriptor table (measured in sectors)
    push %cx
    add $0x400, %bp // bp points to the end of superblock
    push %bp
    movl BlockSize, %ecx
    shr $9, %ecx // divide by 512, which results in the number of sectors
    movl %ecx, SectorPerBlock
    movl StartLBA, %eax
    add %ecx, %eax // add the number of sectors to start LBA
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments
    cmp $0, %ax
    jne FailRead
    movw %bp, BlockGroupPtr // save the pointer to group descriptor table

    // read inode table starting address and compute the LBA
    movl 8(%bp), %eax // inode table starting address
    dec %eax // inode table starts from 0
    mull SectorPerBlock
    add StartLBA, %eax
    movl %eax, %edi

    // compute block group count
    movl 4(%bp), %eax // total number of blocks
    movl 32(%bp), %ecx // number of blocks per group
    divl %ecx // eax = total number of groups
    add $1, %eax // add 1 to total number of groups
    shl $5, %eax // multiply by 32
    add %eax, %ebp // bp = end of group descriptor table

    // read inode table
    movl SectorPerBlock, %ecx
    push %cx
    push %bp
    push %edi
    call ReadLBA
    add $0x8, %sp // pop 3 arguments
    cmp $0, %ax
    jne FailRead

    // we use the second entry, which is root directory
    add $0x80, %bp // bp points to the second entry

    // test the TypeAndPermissions field against 0x4000
    movw 0(%bp), %ax
    movw $0x4000, %cx
    test %cx, %ax
    jz BadFileSystem

    // to search for neldr, we assume searching direct blocks is enough
    mov %bp, %bx // bx points to the second entry. it will facilitate base/index addressing
    add $0x80, %bp // third entry and so on can be overwritten, they are literally free memory

    // this things do not change in the loop, so just push in advance
    movl SectorPerBlock, %ecx
    push %cx
    push %bp
    mov %bp, %dx
    add BlockSize, %dx // dx points to a block
    mov $12, %cx // 12 is the number of direct blocks
SearchRootDir:
    mov %cx, %si // si = 12, 11, 10, ..., 1, 0
    sub $12, %si // si = 12 - cx = 0, 1, 2, ..., 11
    shl $2, %si // si = 0, 4, 8, ..., 44
    mov 40(%bx,%si), %eax
    cmp $0, %eax
    je NextSearchRootDir
    dec %eax // inode number starts from 0
    mull SectorPerBlock
    add StartLBA, %eax
    push %eax
    call ReadLBA
    add $0x4, %sp // pop 1 argument
    cmp $0, %ax
    jne FailRead

    mov %bp, %di // di points to the first entry of the block
IterateEntries:
    movl 0(%di), %eax // inode number
    cmp $0, %eax
    je NextIterateEntries
    movw 4(%di), %ax // total size of the entry
    cmp $0, %ax
    je NextIterateEntries
    movb 7(%di), %al // type indicator
    cmp $2, %al // regular file
    jne ContinueIterateEntries
    movb 6(%di), %al // name length
    push %ax
    mov %di, %ax
    add $8, %ax // ax points to the name
    push %ax
    lea NeldrName, %ax
    push %ax
    call MemCmp
    add $0x6, %sp // pop 3 arguments
    cmp $0, %ax
    jne ContinueIterateEntries
    movl 0(%di), %eax // save the inode number of neldr
    mov %eax, NeldrInode
    jne NextIterateEntries
ContinueIterateEntries:
    movw 4(%di), %ax // total size of the entry
    add %ax, %di // di points to the next entry
    cmp %dx, %di // if di reaches the end of the block
    jae IterateEntries
NextIterateEntries:
    loop SearchRootDir
NextSearchRootDir:
    add $0x4, %sp // pop 2 arguments that pushed before loop

    // if we found neldr?
    mov NeldrInode, %eax
    cmp $0, %eax
    je NeldrNotFound

Spin:
    hlt
    jmp Spin
FailRead:
    lea ReadFailStr, %si
    call PutStr
    jmp Spin
BadFileSystem:
    lea BadFileSystemStr, %si
    call PutStr
    jmp Spin
NeldrNotFound:
    lea NeldrNotFoundStr, %si
    call PutStr
    jmp Spin

ReadLBA:
    // read using LBA in extened mode
    // ReadLBA(DWORD LBA, WORD buffer, WORD size)
    push %bp
    mov %sp, %bp
    pusha

    // LBA to eax
    movl 4(%bp), %eax
    // buffer to bx
    movw 8(%bp), %bx
    // size to cx
    movw 10(%bp), %cx

    // fill a disk address packet in stack
    mov %sp, %di // preserve stack pointer
    sub $0x10, %sp
    mov %sp, %si

    movw %cx, 2(%si) // number of sector
    movw %bx, 4(%si) // offset of buffer
    movl %eax, 8(%si) // low 32bit of relative sector

    xor %dx, %dx
    movw %dx, 6(%si) // segment of buffer is always 0
    movw %dx, 12(%si)
    movw %dx, 14(%si) // upper 16bit of 48bit LBA is always 0

    movb $0x42, %ah // read using LBA in extened mode
    movb $0x80, %dl // drive number

    int $0x13
    mov %di, %sp // restore stack pointer

    jc ErrorReadLBA
    cmp $0x0, %ah // double check success
    jne ErrorReadLBA

    popa
    pop %bp
    mov $0, %ax // return 0 if success
    ret
ErrorReadLBA:
    popa
    pop %bp
    mov $1, %ax
    ret


.section .data
    .align 4

StartLBA:
    .long 0

BlockSize:
    .long 0

SectorPerBlock:
    .long 0

NeldrInode:
    .long 0

BlockGroupPtr:
    .word 0

EnterMsgStr:
    .ascii "..."
    .byte 0

ReadFailStr:
    .ascii "Disk error."
    .byte 0

BadFileSystemStr:
    .ascii "Bad file system."
    .byte 0

NeldrNotFoundStr:
    .ascii "neldr not found."
    .byte 0

NeldrName:
    .ascii "neldr"
    .byte 0