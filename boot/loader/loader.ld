OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(OSLoaderEntry)

SECTIONS
{
	. = 0x1000;

	.text : AT(0x1000) {
		*(.text)
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .debug_* .comment)
	}
}