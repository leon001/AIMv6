OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(__start)

SECTIONS {
	. = 0x80300000;
	.text : {
		*(.text*);
	}
	.rodata : {
		*(.rodata*);
	}
	.data : {
		*(.data*);
	}
	.bss : {
		PROVIDE(__bss_begin__ = .);
		*(.bss*);
		PROVIDE(__bss_end__ = .);
	}
	PROVIDE(__bss_size__ = __bss_end__ - __bss_begin__);
}
