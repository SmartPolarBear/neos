#include "defs.h"

.section .text

// This bootsector parse the ext2 fuckers and find boot files:
// 1) load boot.ini file
// 2) load neldr at HIGHMEM

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
    push %ds
    push %bp
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments

    // calculate block size of ext2 partition
    movl 24(%bp), %ecx // block size
    movl $0x400, %eax // 1024
    shl %ecx, %eax
    movl %eax, BlockSize

    // store inodes per group and blocks per group
    movl 40(%bp), %eax // inodes per group
    movl %eax, InodesPerGroup
    movl 32(%bp), %eax // blocks per group
    movl %eax, BlocksPerGroup

    // save inode size
    movl 76(%bp), %eax
    cmp $1, %eax // if inode size is 128 (version 0)
    jb MajorVersion0
    movw 88(%bp), %ax
    movw %ax, InodeSize
    jmp NextSaveInodeSize
MajorVersion0:
    movw $128, InodeSize
NextSaveInodeSize:

    // read group descriptor table
    mov $0x2, %cx // 2 is the size of group descriptor table (measured in sectors)
    push %cx
    add $0x400, %bp // bp points to the end of superblock
    push %ds
    push %bp
    movl BlockSize, %ecx
    shr $9, %ecx // divide by 512, which results in the number of sectors
    movl %ecx, SectorPerBlock
    movl StartLBA, %eax
    add %ecx, %eax // add the number of sectors to start LBA
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments
    movw %bp, BlockGroupPtr // save the pointer to group descriptor table

    // read inode table starting address and compute the LBA
    movl 8(%bp), %eax // inode table starting address
    call BlockNumToLBA
    movl %eax, %edi

    // compute block group count
    movl 4(%bp), %eax // total number of blocks
    movl 32(%bp), %ecx // number of blocks per group
    divl %ecx // eax = total number of groups
    add $1, %eax // add 1 to total number of groups
    shl $5, %eax // multiply by 32
    add %ax, %bp // bp = end of group descriptor table

    // read inode table
    movl SectorPerBlock, %ecx
    push %cx
    push %ds
    push %bp
    push %edi
    call ReadLBA
    add $0x8, %sp // pop 3 arguments

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
    // re-zero es
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es

    movl SectorPerBlock, %ecx
    push %cx
    push %ds
    push %bp
    mov %bp, %dx
    add BlockSize, %dx // dx points to a block
    xor %cx, %cx
SearchRootDir:
    mov %cx, %si // si = 0, 1, 2, ..., 11
    shl $2, %si // si = 0, 4, 8, ..., 44
    mov 40(%bx,%si), %eax
    cmp $0, %eax
    je NextSearchRootDir
    call BlockNumToLBA
    push %eax
    call ReadLBA
    add $0x4, %sp // pop 1 argument
    mov %bp, %di // di points to the first entry of the block
IterateEntries:
    movl 0(%di), %eax // inode number
    cmp $0, %eax
    je ContinueIterateEntries // do not exit the loop, because there may be empty entries
    movw 4(%di), %ax // total size of the entry
    cmp $0, %ax
    je ContinueIterateEntries
    movb 7(%di), %al // type indicator
    cmp $1, %al // regular file
    jne ContinueIterateEntries

    // search for Neldr
    movb 6(%di), %al // name length
    push %ax
    mov %di, %ax
    add $8, %ax // ax points to the name
    push %ax
    lea NeldrName, %ax
    push %ax
    call MemCmp
    add $0x2, %sp // just pop NeldrName.
    cmp $0, %ax
    je FoundNeldr
    // search for boot.ini
    lea BootIniName, %ax
    push %ax
    call MemCmp
    add $0x6, %sp // pop all arguments
    cmp $0, %ax
    je FoundBootIni
    jmp ContinueIterateEntries
FoundBootIni:
    movl 0(%di), %eax // save the inode number of boot.ini
    mov %eax, BootIniInode
    jmp ContinueIterateEntries
FoundNeldr:
    add $0x4, %sp // pop other arguments
    movl 0(%di), %eax // save the inode number of neldr
    mov %eax, NeldrInode
    jmp ContinueIterateEntries
ContinueIterateEntries:
    addw 4(%di), %di // di points to the next entry
    cmp %dx, %di // if di reaches the end of the block
    jb IterateEntries
NextIterateEntries:
    inc %cx
    cmp $12, %cx // if we have searched all direct blocks
    jb SearchRootDir   // loop inst. won't work, because the body of loop is too long
NextSearchRootDir:
    add $0x4, %sp // pop 2 arguments that pushed before loop

    // if we found boot.ini?
    mov $0x3000, %di // di points to the boot.ini buffer
    mov BootIniInode, %eax
    cmp $0, %eax
    je BootIniNotFound
    call ReadInode

    hlt

    // if we found neldr?
    mov NeldrInode, %eax
    cmp $0, %eax
    je NeldrNotFound

Spin:
    hlt
    jmp Spin
FailRead:
    lea ReadFailStr, %si
    jmp PrintErrMsg
BadFileSystem:
    lea BadFileSystemStr, %si
    jmp PrintErrMsg
NeldrNotFound:
    lea NeldrNotFoundStr, %si
    jmp PrintErrMsg
BootIniNotFound:
    lea BootIniNotFoundStr, %si
    jmp PrintErrMsg
PrintErrMsg:
    call PutStr
    jmp Spin

ReadLBA:
    // read using LBA in extened mode
    // ReadLBA(DWORD LBA, DWORD buffer, WORD size) // buffer = seg:offset
    push %bp
    mov %sp, %bp
    pusha

    // LBA to eax
    movl 4(%bp), %eax
    // buffer to ebx
    movl 8(%bp), %ebx
    // size to cx
    movw 12(%bp), %cx

    // fill a disk address packet in stack
    mov %sp, %di // preserve stack pointer
    sub $0x10, %sp
    mov %sp, %si

    movw %cx, 2(%si) // number of sector
    movl %ebx, 4(%si) // offset of buffer
    movl %eax, 8(%si) // low 32bit of relative sector

    xor %dx, %dx
    movw %dx, 6(%si) // segment of buffer is always 0
    movw %dx, 12(%si)
    movw %dx, 14(%si) // upper 16bit of LBA is always 0

    movb $0x42, %ah // read using LBA in extened mode
    movb $0x80, %dl // drive number

    int $0x13
    mov %di, %sp // restore stack pointer

    jc FailRead
    cmp $0x0, %ah // double check success
    jne FailRead

    popa
    pop %bp
    ret

CalcInodeIndexes:
    // EAX: inode number
    // returns:
    //   EBX: BGDT index
    //   ECX: inode index
    push %eax
    push %edx

    xor %edx, %edx
    dec %eax // inode number starts from 0
    divl InodesPerGroup
    mov %edx, %ecx // inode index is remainder
    mov %eax, %ebx // BGDT index is result

    pop %edx
    pop %eax
    ret

BlockNumToLBA:
    // EAX: block number
    // returns:
    //   EAX: LBA
    push %edx
    dec %eax // block number starts from 0
    mull SectorPerBlock
    add StartLBA, %eax
    pop %edx
    ret

ReadInode:
    // EAX: inode number
    // DS:BP: buffer
    // ES:DI: destination
    pusha

    mov SectorPerBlock, %ecx
    push %cx
    push %ds
    push %bp

    call CalcInodeIndexes

    xor %edx, %edx
    mov %ecx, %eax // eax = inode index
    mulw InodeSize
    divl BlockSize // eax = block offset of inode entry, edx = offset of inode entry

    movw BlockGroupPtr, %si
    shl $5, %ebx // multiply by 32
    add %bx, %si // si points to the BGDT entry
    addl 8(%si), %eax // inode table starting address + block offset of inode entry
    call BlockNumToLBA
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments

    add %dx, %bp
    mov %bp, %si // si points to the inode
    add InodeSize, %bp // bp points to the end of inode

    // get inode size (only low 32bit)
    xor %edx, %edx
    movl 4(%esi), %eax
    divl BlockSize
    inc %eax // add 1 to round up
    mov %eax, %edx

    mov $12, %ecx // 12 is the number of direct blocks
ReadDirect:
    mov SectorPerBlock, %eax
    push %ax
    push %es
    push %di
    // 88 + si -4
    mov $88, %eax
    add %esi, %eax
    mov %ecx, %ebx
    shl $2, %ebx // multiply by 4
    sub %ebx, %eax
    movl (%eax), %eax // eax = direct block number
    call BlockNumToLBA
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments
    add BlockSize, %di // di points to the end of buffer
    dec %edx
    jz ExitReadInode
    loop ReadDirect

    // handle l1 indirect block
    movl SectorPerBlock, %eax
    push %ax
    push %ds
    push %bp
    movl 88(%esi), %eax // l1 indirect block number
    call BlockNumToLBA
    push %eax
    call ReadLBA
    add $0x8, %sp // pop 3 arguments

    mov %bp, %si // si points to the l1 indirect block
    add BlockSize, %bp // bp points to the end of l1 indirect block
    mov %edx, %ecx // rest of the blocks
ReadL1:
    mov SectorPerBlock, %eax
    push %ax
    push %es
    push %di
    mov %edx, %eax
    sub %ecx, %eax // eax = 0, 1, 2, ...
    shl $2, %eax // multiply by 4
    add %esi, %eax
    movl (%eax), %eax // eax = block number
    call BlockNumToLBA
    push %eax
    call ReadLBA
    add $0x10, %sp // pop 3 arguments
    add BlockSize, %di // di points to the end of buffer
    loop ReadL1
ExitReadInode:
    popa
    ret

.section .data
    .align 4

StartLBA:
    .long 0

BlockSize:
    .long 0

InodeSize:
    .word 0

SectorPerBlock:
    .long 0

InodesPerGroup:
    .long 0

BlocksPerGroup:
    .long 0

BootIniInode:
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

BootIniNotFoundStr:
    .ascii "boot.ini not found."
    .byte 0

NeldrName:
    .ascii "neldr"
    .byte 0

BootIniName:
    .ascii "boot.ini"
    .byte 0