#include "defs.h"

.code64
.globl KernelEntry
KernelEntry:
    call KernelMain
Spin:
    jmp Spin