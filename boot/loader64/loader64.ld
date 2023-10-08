OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(OSLoaderEntry)

SECTIONS
{
	. = 0x10000;

    # everything in .text for objcopy
	.text : AT(0x10000) {
		*(.text)
		*(.data)
        *(.rodata)
        *(.bss)
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .note.gnu.property .debug_* .comment)
	}
}