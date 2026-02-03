/* ============================================================================
 * saturn.ld - Sega Saturn Linker Script
 * ============================================================================
 * Memory Layout:
 *   - Boot ROM:    0x00000000 - 0x00080000 (512KB, read-only)
 *   - SMPC:        0x20100000 - 0x20100080
 *   - Backup RAM:  0x00180000 - 0x00200000 (512KB)
 *   - Work RAM Lo: 0x00200000 - 0x00300000 (1MB)
 *   - Work RAM Hi: 0x06000000 - 0x06100000 (1MB)
 *   - VDP1 VRAM:   0x05C00000 - 0x05D00000 (512KB)
 *   - VDP1 FB:     0x05C80000 - 0x05D00000 (512KB)
 *   - VDP2 VRAM:   0x05E00000 - 0x05F00000 (512KB)
 *   - SCSP RAM:    0x25A00000 - 0x25A80000 (512KB)
 * ============================================================================ */

OUTPUT_FORMAT("elf32-sh")
OUTPUT_ARCH(sh)
ENTRY(_start)

MEMORY
{
    /* Work RAM Low - Main program area */
    ram_low (rwx)  : ORIGIN = 0x00200000, LENGTH = 1M

    /* Work RAM High - Stack, heap, and data */
    ram_high (rwx) : ORIGIN = 0x06000000, LENGTH = 1M

    /* VDP1 VRAM */
    vram1 (rw)     : ORIGIN = 0x05C00000, LENGTH = 512K

    /* VDP2 VRAM */
    vram2 (rw)     : ORIGIN = 0x05E00000, LENGTH = 512K

    /* Sound RAM */
    sram (rw)      : ORIGIN = 0x25A00000, LENGTH = 512K
}

SECTIONS
{
    /* ========================================================================
     * Code and Read-Only Data (Work RAM Low)
     * ======================================================================== */

    .text 0x00200000 : {
        _text_start = .;

        /* Entry point and vectors */
        KEEP(*(.text.start))
        KEEP(*(.text.vectors))

        /* Main code */
        *(.text)
        *(.text.*)

        /* Read-only data */
        *(.rodata)
        *(.rodata.*)

        /* C++ constructors/destructors (if used) */
        *(.init)
        *(.fini)

        _text_end = .;
    } > ram_low

    /* Exception handling (if used) */
    .eh_frame : {
        KEEP(*(.eh_frame))
    } > ram_low

    /* ========================================================================
     * Initialized Data (Work RAM High)
     * ======================================================================== */

    .data 0x06000000 : AT(LOADADDR(.text) + SIZEOF(.text)) {
        _data_start = .;

        *(.data)
        *(.data.*)

        /* Small data sections */
        *(.sdata)
        *(.sdata.*)

        _data_end = .;
    } > ram_high

    /* ========================================================================
     * Uninitialized Data (BSS)
     * ======================================================================== */

    .bss (NOLOAD) : {
        _bss_start = .;

        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)

        _bss_end = .;
    } > ram_high

    /* ========================================================================
     * Stack and Heap
     * ======================================================================== */

    /* Stack grows downward from top of RAM */
    _stack_end = ORIGIN(ram_high) + LENGTH(ram_high);
    _stack_start = _stack_end - 64K;  /* 64KB stack */

    /* Heap grows upward from end of BSS */
    _heap_start = _bss_end;
    _heap_end = _stack_start;

    /* ========================================================================
     * Debug Sections (not loaded to target)
     * ======================================================================== */

    /* DWARF debug information */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    .debug_ranges   0 : { *(.debug_ranges) }

    /* Discard unwanted sections */
    /DISCARD/ : {
        *(.note.*)
        *(.comment)
        *(.ARM.*)
    }
}