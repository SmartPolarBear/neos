//
// Created by bear on 9/5/2023.
//
#include "ext.h"
#include "defs.h"
#include "utils.h"
#include "fs.h"
#include "mem.h"
#include "acpi.h"
#include "draw.h"
#include "terminal.h"
#include "param.h"
#include "loader.h"

#include "boot/fs.h"

BOOTPARAMBUF gBootParamBuf;
_Static_assert(((VOID *) &gBootParamBuf.BootParamBuf) == ((VOID *) &gBootParamBuf.BootParam),
               "BootParamBuf and BootParam are not at the same address.");

void ExitBootServices() {
    TerminalWriteString("Exiting boot services...");
}


void MakeKernelBootParams() {
    gBootParam.BootService.ExitBootServices = ExitBootServices;
}


// NELDR do following things:
// 0) Initialize memory stuffs
// 1) first, load kernel binaries and HAL binaries into memory.
// 2) place physical memory pages just after kernel binaries.
// 3) scan hardware and place information after pages with ACPI
// 4) jmp to kernel entry
char *testText = "FUCK YOU! TEST FUCK!";
char *testText2 = "Your mother dead!";

// TODO: .data seems corrupted or not loaded correctly.
UINT_PTR LoaderMain64(UINT_PTR bufferTop, UINT_PTR activePartAddr) {
    // boot-time terminal
    InitializeTerminal();
    TerminalClear();
    FillScreen(R(BLUE), G(BLUE), B(BLUE));
    TerminalWriteString("NEOS is scanning hardware...");
    for (;;);

    // boot-time memory allocator
    InitializeMemory((BYTE *) bufferTop);

    // Boot-time file system
    InitializeBootFs((PARTTABLEITEM *) activePartAddr);

    // ACPI
    InitializeAcpi();

    // NeosExecutive (kernal, HAL, etc.).
    // HAL will be responsible for initializing SMP.
    UINT_PTR kernEntry = LoadKernel();
    LoadHal();

    // NeosExecutive drivers based on ACPI information
    AcpiLoadDriverForDevices();

    // Place memory pages
    InitializeMemoryPages();

    // Memory post-initialization
    PostInitializeMemory();

    // Make kernel boot parameters
    MakeKernelBootParams();

    // kernel entry point will be saved to register rax, where head64.S will jmp to.
    // head64.S will also save the address of boot parameters to register rdi.
    return kernEntry;
}

void NO_RETURN FailToGotoKernel() // will be called by head64.S if kernel loading failed.
{
    Panic("Cannot jump to kernel.");
}

