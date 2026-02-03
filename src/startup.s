! ============================================================================
! startup.s - Sega Saturn SH-2 Startup Code
! ============================================================================
! This is the entry point for the Saturn program
! ============================================================================

    .section .text.start
    .global _start

_start:
    ! Disable interrupts
    mov.l   sr_mask, r0
    ldc     r0, sr

    ! Set up stack pointer (top of Work RAM High)
    mov.l   stack_addr, r15

    ! Set up VBR (Vector Base Register) to Work RAM High
    mov.l   vbr_addr, r0
    ldc     r0, vbr

    ! Initialize .data section (copy from ROM to RAM)
    mov.l   data_start, r0      ! Source (in ROM)
    mov.l   data_end, r1        ! Destination start
    mov.l   data_size, r2       ! Size

    tst     r2, r2              ! Check if size > 0
    bt      clear_bss

copy_data_loop:
    mov.l   @r0+, r3
    mov.l   r3, @r1
    add     #4, r1
    dt      r2
    bf      copy_data_loop

clear_bss:
    ! Clear .bss section (uninitialized data)
    mov.l   bss_start, r0
    mov.l   bss_end, r1
    mov     #0, r2

clear_bss_loop:
    cmp/eq  r0, r1
    bt      call_main
    mov.l   r2, @r0
    add     #4, r0
    bra     clear_bss_loop
    nop

call_main:
    ! Call main function
    mov.l   main_addr, r0
    jsr     @r0
    nop

    ! If main returns, loop forever
infinite_loop:
    bra     infinite_loop
    nop

! ============================================================================
! Slave CPU Entry Point
! ============================================================================
    .global _slave_entry
_slave_entry:
    ! Disable interrupts
    mov.l   sr_mask, r0
    ldc     r0, sr

    ! Set up slave stack pointer
    mov.l   slave_stack_addr, r15

    ! Set up VBR
    mov.l   vbr_addr, r0
    ldc     r0, vbr

    ! Call slave main function
    mov.l   slave_main_addr, r0
    jsr     @r0
    nop

slave_loop:
    bra     slave_loop
    nop

! ============================================================================
! Constants and Addresses
! ============================================================================
    .align 4

sr_mask:
    .long   0x000000F0          ! Disable all interrupts

stack_addr:
    .long   0x06100000          ! Top of Work RAM High (1MB)

slave_stack_addr:
    .long   0x060F0000          ! Slave stack (64KB below master)

vbr_addr:
    .long   0x06000000          ! Vector Base Register

data_start:
    .long   _data_start

data_end:
    .long   _data_end

data_size:
    .long   (_data_end - _data_start) / 4

bss_start:
    .long   _bss_start

bss_end:
    .long   _bss_end

main_addr:
    .long   _main

slave_main_addr:
    .long   _slave_main

! ============================================================================
! Interrupt Vector Table
! ============================================================================
    .section .text.vectors
    .global _vectors

_vectors:
    ! Power-on reset vector
    .long   _start              ! 0x00: Power-on PC
    .long   0x06100000          ! 0x04: Power-on SP

    ! Exception vectors
    .long   _start              ! 0x08: Manual reset PC
    .long   0x06100000          ! 0x0C: Manual reset SP
    .long   default_handler     ! 0x10: Illegal instruction
    .long   0                   ! 0x14: Reserved
    .long   default_handler     ! 0x18: Slot illegal instruction
    .long   0                   ! 0x1C: Reserved
    .long   default_handler     ! 0x20: CPU address error
    .long   default_handler     ! 0x24: DMA address error
    .long   default_handler     ! 0x28: NMI
    .long   default_handler     ! 0x2C: User break

    ! Reserved vectors (0x30-0x3C)
    .space  16

    ! Interrupt vectors (0x40+)
    .long   vblank_in_handler   ! 0x40: V-Blank-IN
    .long   vblank_out_handler  ! 0x44: V-Blank-OUT
    .long   hblank_in_handler   ! 0x48: H-Blank-IN
    .long   timer0_handler      ! 0x4C: Timer 0
    .long   timer1_handler      ! 0x50: Timer 1
    .long   dsp_end_handler     ! 0x54: DSP end
    .long   sound_req_handler   ! 0x58: Sound request
    .long   system_manager_handler ! 0x5C: System manager
    .long   pad_handler         ! 0x60: PAD interrupt
    .long   level2_dma_handler  ! 0x64: Level 2 DMA end
    .long   level1_dma_handler  ! 0x68: Level 1 DMA end
    .long   level0_dma_handler  ! 0x6C: Level 0 DMA end
    .long   dma_illegal_handler ! 0x70: DMA illegal

! ============================================================================
! Default Interrupt Handler
! ============================================================================
default_handler:
hblank_in_handler:
timer0_handler:
timer1_handler:
dsp_end_handler:
sound_req_handler:
system_manager_handler:
pad_handler:
level2_dma_handler:
level1_dma_handler:
level0_dma_handler:
dma_illegal_handler:
    rte
    nop

! ============================================================================
! Weak symbols for handlers (can be overridden)
! ============================================================================
    .weak   vblank_in_handler
    .weak   vblank_out_handler
    .weak   _slave_main

! If not defined elsewhere, use default
vblank_in_handler = default_handler
vblank_out_handler = default_handler
_slave_main = slave_loop