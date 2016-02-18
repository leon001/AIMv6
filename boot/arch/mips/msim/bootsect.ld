OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(boot)

SECTIONS {
	.text : {
		*(.text*);
	}
}
