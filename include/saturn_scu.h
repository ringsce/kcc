//
// Created by Pedro Dias Vicente on 15/01/2026.
//

// ============================================================================
// include/saturn_scu.h - SCU (System Control Unit) Definitions
// ============================================================================
#ifndef SATURN_SCU_H
#define SATURN_SCU_H

#include <stdint.h>

// DMA Channels
typedef enum {
    SCU_DMA_CH0 = 0,
    SCU_DMA_CH1 = 1,
    SCU_DMA_CH2 = 2
} SCUDMAChannel;

// DMA Transfer Modes
typedef enum {
    SCU_DMA_MODE_DIRECT = 0,
    SCU_DMA_MODE_INDIRECT = 1
} SCUDMAMode;

void scu_init(void);
void scu_dma_transfer(SCUDMAChannel ch, void *src, void *dst,
                      uint32_t size, SCUDMAMode mode);
void scu_dma_wait(SCUDMAChannel ch);
void scu_interrupt_mask(uint32_t mask);
void scu_interrupt_enable(uint32_t interrupts);

#endif // SATURN_SCU_H