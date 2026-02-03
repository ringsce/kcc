// saturn_crt0.s - C runtime startup for Sega Saturn

    .section .text
    .global _start
    .align 2

_start:
    ! Initialize stack pointer
    mov.l   stack_addr,r15

    ! Clear BSS section
    mov.l   bss_start_addr,r0
    mov.l   bss_end_addr,r1
    mov     #0,r2
clear_bss_loop:
    cmp/eq  r0,r1
    bt      bss_cleared
    mov.l   r2,@r0
    add     #4,r0
    bra     clear_bss_loop
    nop

bss_cleared:
    ! Initialize Saturn hardware
    bsr     _saturn_init
    nop

    ! Call global constructors (if any)
    bsr     _init
    nop

    ! Call main
    bsr     _main
    nop

    ! If main returns, loop forever
_exit:
    bra     _exit
    nop

    .align 4
stack_addr:
    .long   0x060FFFF0      ! Stack top address
bss_start_addr:
    .long   _bss_start
bss_end_addr:
    .long   _bss_end
