OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(HalEntry)

SECTIONS
{
	/* Link the HAL at this address: "." means the current address */
    /* Must be equal to HALLINK */
	. = 0xffffffff8A000000;

	.text : {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}

	.rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    .data : {
        *(.data .data.* .gnu.linkonce.d.*)
    }

    .bss : {
        *(.bss .bss.* .gnu.linkonce.b.*)
    }

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}
}