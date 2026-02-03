! ===========================================================================
! saturn_sdk/crt0.s - Saturn C Runtime Startup
! ===========================================================================

    .section .text
    .global _start
    .global _stack_top
    .align 2

_start:
    ! Initialize stack pointer (top of high work RAM)
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
    mov.l   saturn_init_addr,r0
    jsr     @r0
    nop

    ! Call global constructors (C++ support)
    mov.l   init_addr,r0
    tst     r0,r0
    bt      no_init
    jsr     @r0
    nop

no_init:
    ! Call main function
    mov.l   main_addr,r0
    jsr     @r0
    nop

    ! If main returns, loop forever
_exit:
    bra     _exit
    nop

    ! Data section alignment
    .align 4

stack_addr:
    .long   0x060FFFF0          ! Stack top in high work RAM

bss_start_addr:
    .long   __bss_start

bss_end_addr:
    .long   __bss_end

saturn_init_addr:
    .long   _saturn_init

init_addr:
    .long   __init_array_start

main_addr:
    .long   _main

! ===========================================================================
! Exception and Interrupt Vectors
! ===========================================================================

    .section .vectors,"ax"
    .global _vectors

_vectors:
    .long   _start              ! Power-on reset
    .long   _start              ! Manual reset
    .long   _default_handler    ! Illegal instruction
    .long   _default_handler    ! Slot illegal instruction
    .long   _default_handler    ! CPU address error
    .long   _default_handler    ! DMA address error
    .long   _default_handler    ! NMI
    .long   _default_handler    ! User break
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved
    .long   _default_handler    ! Reserved

    ! Level 0-15 interrupts (64 vectors)
    .rept   64
    .long   _default_handler
    .endr

_default_handler:
    rte
    nop