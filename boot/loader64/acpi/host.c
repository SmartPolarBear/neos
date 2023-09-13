//
// Created by bear on 9/13/2023.
//

#include "draw.h"
#include "terminal.h"
#include "mem.h"
#include "utils.h"

#include "lai/host.h"

/* Logs a message. level can either be LAI_DEBUG_LOG for debugging info,
   or LAI_WARN_LOG for warnings */
void laihost_log(int level, const char* msg)
{
	switch (level)
	{
	case LAI_DEBUG_LOG:
		TerminalWriteString("[DEBUG] ");
		break;
	case LAI_WARN_LOG:
		TerminalWriteString("[WARN] ");
		break;
	}
	TerminalWriteString((char*)msg);
}

/* Reports a fatal error, and halts. */
__attribute__((noreturn)) void laihost_panic(const char* msg)
{
	Panic((char*)msg);
}

/* Self-explanatory */
void* laihost_malloc(size_t size)
{
	AllocateLow(PGROUNDUP(size));
}

void* laihost_realloc(void* oldptr, size_t newsize, size_t oldsize)
{
	AllocateLow(PGROUNDUP(newsize));
}

void laihost_free(void* ptr, size_t size)
{
	// Do nothing.
}

/* Maps count bytes from the given physical address and returns
   a virtual address that can be used to access the memory. */
void* laihost_map(size_t address, size_t count)
{
	return (void*)address; // we are in fucking linear mapped mode.
}

/* Unmaps count bytes from the given virtual address.
   LAI only calls this on memory that was previously mapped by laihost_map(). */
void laihost_unmap(void* pointer, size_t count)
{
	// Do nothing.
}