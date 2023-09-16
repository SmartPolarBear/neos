#include "defs.h"

.code64
.globl KernelEntry
KernelEntry:
Spin:
    jmp Spin