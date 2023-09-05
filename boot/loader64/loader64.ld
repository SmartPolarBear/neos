OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(OSLoaderEntry)

SECTIONS
{
	. = 0x80000;

    # everything in .text for objcopy
	.text : AT(0x80000) {
		*(.text)
		*(.data)
        *(.rodata)
        *(.bss)
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .debug_* .comment)
	}
}