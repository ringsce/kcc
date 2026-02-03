// ============================================================================
// saturn_sdk/src/scu.c - SCU Implementation
// ============================================================================
#include "saturn_scu.h"

void scu_init(void) {
    // Disable all DMA channels
    SCU_D0EN = 0;

    // Mask all interrupts initially
    SCU_IMS = 0xFFFFFFFF;
}

void scu_dma_transfer(SCUDMAChannel ch, void *src, void *dst,
                      uint32_t size, SCUDMAMode mode) {
    volatile uint32_t *dma_regs = (volatile uint32_t*)(SCU_BASE + (ch * 0x20));

    // Set source and destination
    dma_regs[0] = (uint32_t)src;
    dma_regs[1] = (uint32_t)dst;
    dma_regs[2] = size;

    // Set mode and start
    dma_regs[5] = (mode << 24) | 0x101;
}

void scu_dma_wait(SCUDMAChannel ch) {
    volatile uint32_t *dma_regs = (volatile uint32_t*)(SCU_BASE + (ch * 0x20));
    while (dma_regs[5] & 0x02);
}

void scu_interrupt_mask(uint32_t mask) {
    SCU_IMS = mask;
}

void scu_interrupt_enable(uint32_t interrupts) {
    SCU_IMS &= ~interrupts;
}