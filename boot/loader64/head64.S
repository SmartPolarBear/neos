#include "defs.h"

.code32
.globl OSLoaderEntry
OSLoaderEntry:
    mov $0x7c00, %esp
    push %ebx # save active partition

# zero pages for our bootstrap page tables
    xor %eax, %eax
    mov $IDENTITY_PAGING_BASE, %edi
    mov $IDENTITY_PAGING_SIZE, %ecx # PGDIR size
    rep stosb
    mov $IDENTITY_PAGING_BASE, %edi

# reserve pml4t sapce
    mov %edi, %edx      # edx is pointing to P4ML
    add $0x1000, %edi   # edi is pointing to PDPT-A

# PML4[0] -> PDPT-A
    mov %edi, %eax
    or $0x3, %eax
    mov %eax, (%edx)  # write to P4ML[0]
    mov %edx, %ecx    # ecx is pointing to P4ML[0]
    add $0xFF8, %ecx  # ecx is pointing to P4ML[511]
    mov %edi, %edx    # now edx is pointing to PDPT-A
    add $0x1000, %edi # edi is pointing to PDPT-B

# PML4[511] -> PDPT-B
    mov %edi, %eax
    or $0x3, %eax
    mov %eax, (%ecx)  # write to P4ML[511]
    mov %edi, %ebx    # now ebx is pointing to PDPT-B
    add $0xFF0, %ebx  # ebx is pointing to PDPT-B[510]
    add $0x1000, %edi # edi is pointing to PD1

# PDPT-A[0...4] -> PD1...PD4; PDPT-B[510..511] -> PD1...PD2
    mov %edi, %esi # temporary save edi
    mov $4, %ecx
FillPDPT:
    mov %edi, %eax
    or $0x3, %eax
    mov %eax, (%edx)  # write to PDPT-A
    cmp $2, %ecx      # write to PDPT-B for first 2 PD
    jle NextPDPTB
    mov %eax, (%ebx)
    add $0x8, %ebx    # ebx is pointing to next PDPT-B entry
NextPDPTB:
    add $0x1000, %edi # edi is pointing to next PD
    add $0x8, %edx    # edx is pointing to next PDPT-A entry
    loop FillPDPT
    push %edi # save edi
    mov %esi, %edi # restore edi

# Identity paging for first 4GB, no cache and write-through
    mov $0x9B, %eax
    mov %edi, %ebx
    mov $(512*4), %ecx
ptbl_loop:
    mov %eax, (%ebx)
    add $0x200000, %eax
    add $0x8, %ebx
    dec %ecx
    jnz ptbl_loop



# CR3 -> PML4
    mov $BUFFER_START, %eax
    mov %eax, %cr3

    lgdt gdtr64

# Enable PAE - CR4.PAE=1
    mov %cr4, %eax
    bts $5, %eax
    mov %eax, %cr4

# enable long mode - EFER.LME=1
    mov $0xc0000080, %ecx
    rdmsr
    bts $8, %eax
    wrmsr

# enable paging
    mov %cr0, %eax
    bts $31, %eax
    mov %eax, %cr0

    pop %edi # restore edi, and rdi will be first argument for C code
    add $0x1000, %edi

    pop %esi # restore esi, and rsi will be second argument for C code

# shift to 64bit segment
    ljmp $8,$(entry64low)

.align 16
gdtr64:
    .word gdt64_end - gdt64_begin - 1;
    .quad gdt64_begin

.align 16
gdt64_begin:
    .long 0x00000000 # 0: null desc
    .long 0x00000000
    .long 0x00000000 # 1: Code, R/X, Nonconforming
    .long 0x00209800
    .long 0x00000000 # 2: Data, R/W, Expand Down
    .long 0x00009000
gdt64_end:

.align 16
.code64
entry64low:
    movq $entry64high, %rax
    jmp *%rax

entry64high:
# ensure data segment registers are sane
    xor %rax, %rax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov $0xdeadbeef, %rax
    call LoaderMain64

// if not do this silly thing, gcc will fucking complian "operand size mismatch"
    mov $0xdeadbeef, %rbx
    cmp %rbx, %rax
    je FailToGotoKernel
    jmp *%rax // jmp to kernel entry point

Spin:
    hlt
    jmp Spin