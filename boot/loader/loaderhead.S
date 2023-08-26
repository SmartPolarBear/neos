.code16
.globl OSLoaderEntry
OSLoaderEntry:
    // set boot stack
    mov $0x7c00, %sp

    cli
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

  # Physical address line A20 is tied to zero so that the first PCs
  # with 2 MB would run software that assumed 1 MB.  Undo that.
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

// save memory map at 0x8000
ProbeMem:
    movl $0, 0x8000
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
    // initialize VESA
    call InitializeVesa



Spin:
    hlt
    jmp Spin


