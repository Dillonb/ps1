#include "dma.h"

#include <log.h>
#include <mem/addresses.h>
#include <mem/ps1system.h>

void dma_register_write(u32 address, u32 value) {
    switch (address) {
        case DMA_DPCR:
            PS1SYS.dpcr = value;
            break;
        case DMA_DICR:
            PS1SYS.dicr = value;
            break;
        case DMA0_BASE_ADDR:
            logfatal("DMA0_BASE_ADDR = %08X", value);
        case DMA0_BLOCK_CTRL:
            logfatal("DMA0_BLOCK_CTRL = %08X", value);
        case DMA0_CHANNEL_CTRL:
            logfatal("DMA0_CHANNEL_CTRL = %08X", value);
        case DMA1_BASE_ADDR:
            logfatal("DMA1_BASE_ADDR = %08X", value);
        case DMA1_BLOCK_CTRL:
            logfatal("DMA1_BLOCK_CTRL = %08X", value);
        case DMA1_CHANNEL_CTRL:
            logfatal("DMA1_CHANNEL_CTRL = %08X", value);
        case DMA2_BASE_ADDR:
            logwarn("DMA2_BASE_ADDR = %08X", value);
            break;
        case DMA2_BLOCK_CTRL:
            logwarn("DMA2_BLOCK_CTRL = %08X", value);
            break;
        case DMA2_CHANNEL_CTRL:
            logwarn("DMA2_CHANNEL_CTRL = %08X", value);
            break;
        case DMA3_BASE_ADDR:
            logfatal("DMA3_BASE_ADDR = %08X", value);
        case DMA3_BLOCK_CTRL:
            logfatal("DMA3_BLOCK_CTRL = %08X", value);
        case DMA3_CHANNEL_CTRL:
            logfatal("DMA3_CHANNEL_CTRL = %08X", value);
        case DMA4_BASE_ADDR:
            logfatal("DMA4_BASE_ADDR = %08X", value);
        case DMA4_BLOCK_CTRL:
            logfatal("DMA4_BLOCK_CTRL = %08X", value);
        case DMA4_CHANNEL_CTRL:
            logfatal("DMA4_CHANNEL_CTRL = %08X", value);
        case DMA5_BASE_ADDR:
            logfatal("DMA5_BASE_ADDR = %08X", value);
        case DMA5_BLOCK_CTRL:
            logfatal("DMA5_BLOCK_CTRL = %08X", value);
        case DMA5_CHANNEL_CTRL:
            logfatal("DMA5_CHANNEL_CTRL = %08X", value);
        case DMA6_BASE_ADDR:
            logwarn("DMA6_BASE_ADDR = %08X", value);
            break;
        case DMA6_BLOCK_CTRL:
            logwarn("DMA6_BLOCK_CTRL = %08X", value);
            break;
        case DMA6_CHANNEL_CTRL:
            logwarn("DMA6_CHANNEL_CTRL = %08X", value);
            break;
        default:
            logfatal("Unknown DMA register write: [%08X]=%08X", address, value);
    }
}

u32 dma_register_read(u32 address) {
    switch (address) {
        case DMA_DPCR:
            return PS1SYS.dpcr;
        case DMA_DICR:
            return PS1SYS.dicr;
        case DMA0_BASE_ADDR:
            logfatal("read DMA0_BASE_ADDR");
        case DMA0_BLOCK_CTRL:
            logfatal("read DMA0_BLOCK_CTRL");
        case DMA0_CHANNEL_CTRL:
            logfatal("read DMA0_CHANNEL_CTRL");
        case DMA1_BASE_ADDR:
            logfatal("read DMA1_BASE_ADDR");
        case DMA1_BLOCK_CTRL:
            logfatal("read DMA1_BLOCK_CTRL");
        case DMA1_CHANNEL_CTRL:
            logfatal("read DMA1_CHANNEL_CTRL");
        case DMA2_BASE_ADDR:
            logfatal("read DMA2_BASE_ADDR");
        case DMA2_BLOCK_CTRL:
            logfatal("read DMA2_BLOCK_CTRL");
        case DMA2_CHANNEL_CTRL:
            logwarn("read DMA2_CHANNEL_CTRL");
            return 0x00000000;
        case DMA3_BASE_ADDR:
            logfatal("read DMA3_BASE_ADDR");
        case DMA3_BLOCK_CTRL:
            logfatal("read DMA3_BLOCK_CTRL");
        case DMA3_CHANNEL_CTRL:
            logfatal("read DMA3_CHANNEL_CTRL");
        case DMA4_BASE_ADDR:
            logfatal("read DMA4_BASE_ADDR");
        case DMA4_BLOCK_CTRL:
            logfatal("read DMA4_BLOCK_CTRL");
        case DMA4_CHANNEL_CTRL:
            logfatal("read DMA4_CHANNEL_CTRL");
        case DMA5_BASE_ADDR:
            logfatal("read DMA5_BASE_ADDR");
        case DMA5_BLOCK_CTRL:
            logfatal("read DMA5_BLOCK_CTRL");
        case DMA5_CHANNEL_CTRL:
            logfatal("read DMA5_CHANNEL_CTRL");
        case DMA6_BASE_ADDR:
            logfatal("read DMA6_BASE_ADDR");
        case DMA6_BLOCK_CTRL:
            logfatal("read DMA6_BLOCK_CTRL");
        case DMA6_CHANNEL_CTRL:
            logwarn("read DMA6_CHANNEL_CTRL");
            return 0x00000000;
        default:
            logfatal("Unknown DMA register read: [%08X]", address);
    }
}