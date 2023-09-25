OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(Ext2BootEntry)

SECTIONS
{
	. = 0x1000;

    # everything in .text for objcopy
	.text : AT(0x1000) {
		*(.text)
		*(.data)
        *(.rodata)
        *(.bss)
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .debug_* .comment)
	}
}