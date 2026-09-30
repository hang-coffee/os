ENTRY(_start)

SECTIONS
{
    KERNEL_BASE = 0xc0000000;

    . = 0x200000;

    .multiboot ALIGN(4) :
    {
        KEEP(*(.multiboot))
    }

    .boot ALIGN(16) :
    {
        KEEP(*(.boot))
    }

    _boot_end = ALIGN(4096);

    . = KERNEL_BASE + _boot_end;
    kernel_start = .;

    .text ALIGN(4096) : AT(ADDR(.text) - KERNEL_BASE)
    {
        *(.text)
        *(.text.*)
    }
    .rodata ALIGN(4096) : AT(ADDR(.rodata) - KERNEL_BASE)
    {
        *(.rodata)
        *(.rodata.*)
    }
    .data ALIGN(4096) : AT(ADDR(.data) - KERNEL_BASE)
    {
        *(.data)
        *(.data.*)
    }
    .bss ALIGN(4096) : AT(ADDR(.bss) - KERNEL_BASE)
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }
	.stack ALIGN(4096) : AT(ADDR(.stack) - KERNEL_BASE)
	{
 	   *(.stack)
	}


    kernel_end = .;
    kernel_phys_start = 0x200000;
    kernel_phys_end   = kernel_end - KERNEL_BASE;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.gcc_except_table*)
    }
}