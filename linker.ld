ENTRY(_start)

SECTIONS
{
	. = 0x100000;
	kernel_start = .;
	.multiboot ALIGN(4) :
	{
		KEEP(*(.multiboot))
	}
	.text ALIGN(4096) : 
	{ 
		*(.text) 
		*(.text.*)
	}
	.rodata ALIGN(4096) :
	{
		*(.rodata)
		*(.rodata.*)
	}
	.data ALIGN(4096) :
	{
		*(.data)
		*(.data.*)
	}
	.bss ALIGN(4096) :
	{
		__bss_start = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		__bss_end = .;
	}
	kernel_end = .;
	/DISCARD/ :
	{
		*(.comment)
		*(.note*)
		*(.eh_frame*)
		*(.gcc_except_table*)
	}
}

