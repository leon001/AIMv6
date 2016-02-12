OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(reset)

MEMORY {
	rom (RX) : ORIGIN = 0xbfc00000, LENGTH = 1M
	ram (RW) : ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS {
	.text : {
		__text_begin__ = .;
		*(.text.reset);
		*(.text*);
		__text_end__ = .;
	} > rom

	.rodata : {
		__rodata_begin__ = .;
		*(.rodata*);
		__rodata_end__ = .;
	} > rom

	PROVIDE(__rodata_size__ = SIZEOF(.rodata));

	.data : AT(__rodata_end__) {
		__data_begin__ = .;
		*(.data);
		__data_end__ = .;
	} > ram
	PROVIDE(__data_size__ = SIZEOF(.data));

	.bss : {
		__bss_begin__ = .;
		*(.bss);
		__bss_end__ = .;
	}
	PROVIDE(__bss_size__ = SIZEOF(.bss));

	/DISCARD/ : {
		*(.reginfo);
		*(.MIPS.abiflags);
	}
}
