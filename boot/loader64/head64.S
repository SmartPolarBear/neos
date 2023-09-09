.code32
.globl OSLoaderEntry
OSLoaderEntry:
    mov $0x7c00, %esp

    xor %eax, %eax
# zero 5 pages for our bootstrap page tables
    xor %eax, %eax
    mov $0x1000, %edi
    mov $0x6000, %ecx
    rep stosb

# P4ML[0] -> 0x2000 (PDPT-A)
    mov $(0x2000 | 3), %eax
    mov %eax, 0x1000

# PDPT-A[0] -> 0x3000 (PD1)
    mov $(0x3000 | 3), %eax
    mov %eax, 0x2000

# PDPT-A[1] -> 0x4000 (PD2)
    mov $(0x4000 | 3), %eax
    mov %eax, 0x2008

# PDPT-A[2] -> 0x5000 (PD3)
    mov $(0x5000 | 3), %eax
    mov %eax, 0x2010

# PDPT-A[3] -> 0x6000 (PD4)
    mov $(0x6000 | 3), %eax
    mov %eax, 0x2018

# Identity paging for first 4GB, no cache and write-through
    mov $0x9B, %eax
    mov $0x3000, %ebx

    mov $(512*4), %ecx
ptbl_loop:
    mov %eax, (%ebx)
    add $0x200000, %eax
    add $0x8, %ebx
    dec %ecx
    jnz ptbl_loop

# CR3 -> 0x1000 (P4ML)
    mov $0x1000, %eax
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

    // It won't return, so jmp instead of call
    jmp LoaderMain64

Spin:
    hlt
    jmp Spin