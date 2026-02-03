! ===========================================================================
! saturn_sdk/saturn.ld - Linker Script for Sega Saturn
! ===========================================================================

/*
 * Sega Saturn Memory Layout:
 *
 * 0x00000000 - 0x000FFFFF  : Boot ROM (1MB)
 * 0x00200000 - 0x002FFFFF  : Work RAM Low (1MB)
 * 0x05A00000 - 0x05AFFFFF  : CD-ROM (?)
 * 0x05C00000 - 0x05C7FFFF  : VDP1 VRAM (512KB)
 * 0x05C80000 - 0x05CFFFFF  : VDP1 Frame Buffer (512KB)
 * 0x05E00000 - 0x05EFFFFF  : VDP2 VRAM (512KB)
 * 0x05F00000 - 0x05F7FFFF  : VDP2 CRAM (4KB)
 * 0x06000000 - 0x060FFFFF  : Work RAM High (1MB)
 * 0x25800000 - 0x258FFFFF  : SCSP (Sound)
 * 0x25C00000 - 0x25CFFFFF  : VDP1 Registers
 * 0x25F00000 - 0x25F7FFFF  : VDP2 Registers
 * 0x25FE0000 - 0x25FEFFFF  : SCU Registers
 */

OUTPUT_FORMAT("elf32-sh")
OUTPUT_ARCH(sh)
ENTRY(_start)

MEMORY
{
    /* Boot area - actual ROM or loaded from CD */
    BOOT (rx)   : ORIGIN = 0x00200000, LENGTH = 0x00100000  /* Work RAM Low */

    /* High work RAM for stack and heap */
    HWRAM (rwx) : ORIGIN = 0x06000000, LENGTH = 0x00100000  /* Work RAM High */
}

/* Stack size configuration */
_stack_size = 0x10000;  /* 64KB stack */
_heap_size  = 0x40000;  /* 256KB heap */

SECTIONS
{
    /* Interrupt vectors must be at the start */
    .vectors : ALIGN(4) {
        KEEP(*(.vectors))
        . = ALIGN(4);
    } > BOOT

    /* Code section */
    .text : ALIGN(4) {
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)

        /* Read-only data */
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)

        . = ALIGN(4);
        _etext = .;
    } > BOOT

    /* Constructor and destructor tables */
    .init_array : ALIGN(4) {
        __init_array_start = .;
        KEEP(*(.init_array))
        KEEP(*(.ctors))
        __init_array_end = .;
    } > BOOT

    .fini_array : ALIGN(4) {
        __fini_array_start = .;
        KEEP(*(.fini_array))
        KEEP(*(.dtors))
        __fini_array_end = .;
    } > BOOT

    /* Initialized data section */
    .data : ALIGN(4) {
        _data_start = .;
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(4);
        _edata = .;
    } > BOOT

    /* Small data section */
    .sdata : ALIGN(4) {
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(4);
    } > BOOT

    /* Uninitialized data section */
    .bss (NOLOAD) : ALIGN(4) {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > HWRAM

    /* Heap section */
    .heap (NOLOAD) : ALIGN(4) {
        __heap_start = .;
        . = . + _heap_size;
        __heap_end = .;
    } > HWRAM

    /* Stack section (grows downward) */
    .stack (NOLOAD) : ALIGN(8) {
        __stack_bottom = .;
        . = . + _stack_size;
        __stack_top = .;
        _stack_top = .;
    } > HWRAM

    /* Debug sections */
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_ranges   0 : { *(.debug_ranges) }

    /* Discard these sections */
    /DISCARD/ : {
        *(.note.*)
        *(.comment)
        *(.eh_frame)
    }
}

/* Provide symbols for runtime use */
PROVIDE(__executable_start = ORIGIN(BOOT));
PROVIDE(__text_start = ADDR(.text));
PROVIDE(__text_end = ADDR(.text) + SIZEOF(.text));
PROVIDE(__data_start = ADDR(.data));
PROVIDE(__data_end = ADDR(.data) + SIZEOF(.data));
PROVIDE(__bss_size = SIZEOF(.bss));
PROVIDE(__heap_size = _heap_size);
PROVIDE(__stack_size = _stack_size);