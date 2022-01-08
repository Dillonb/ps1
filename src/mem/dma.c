#include "dma.h"

#include <log.h>
#include <mem/addresses.h>
#include <mem/ps1system.h>

void write_dma_channel_ctrl(int channel, u32 value) {
    PS1SYS.dma.dma_channel_ctrl[channel].raw = value;
    dma_channel_ctrl_t ctrl = PS1SYS.dma.dma_channel_ctrl[channel];

    logwarn("DMA%d_CHANNEL_CTRL.direction: %d (%s)", channel, ctrl.direction, ctrl.direction == 0 ? "to main ram" : "from main ram");
    logwarn("DMA%d_CHANNEL_CTRL.reverse: %d (%s)", channel, ctrl.reverse, ctrl.reverse == 0 ? "add 4 between each transfer" : "subtract 4 between each transfer");
    logwarn("DMA%d_CHANNEL_CTRL.chopping: %d", channel, ctrl.chopping);
    logwarn("DMA%d_CHANNEL_CTRL.syncmode: %d", channel, ctrl.syncmode);
    logwarn("DMA%d_CHANNEL_CTRL.chopping_dma_window_size: %d", channel, ctrl.chopping_dma_window_size);
    logwarn("DMA%d_CHANNEL_CTRL.chopping_cpu_window_size: %d", channel, ctrl.chopping_cpu_window_size);
    logwarn("DMA%d_CHANNEL_CTRL.start_busy: %d", channel, ctrl.start_busy);
    logwarn("DMA%d_CHANNEL_CTRL.start_trigger: %d", channel, ctrl.start_trigger);

    unimplemented(ctrl.start_busy != 0, "DMA%d_CHANNEL_CTRL: start_busy = %d", channel, ctrl.start_busy);
    unimplemented(ctrl.start_trigger != 0, "DMA%d_CHANNEL_CTRL: start_trigger = %d", channel, ctrl.start_trigger);
}

void dma_register_write(u32 address, u32 value) {
    switch (address) {
        case DMA_DPCR:
            PS1SYS.dma.dpcr = value;
            break;
        case DMA_DICR:
            PS1SYS.dma.dicr = value;
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
            logfatal("DMA2_BASE_ADDR = %08X", value);
        case DMA2_BLOCK_CTRL:
            logfatal("DMA2_BLOCK_CTRL = %08X", value);
        case DMA2_CHANNEL_CTRL:
            logwarn("DMA2_CHANNEL_CTRL = %08X", value);
            write_dma_channel_ctrl(2, value);
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
            PS1SYS.dma.base_addr[6] = value;
            break;
        case DMA6_BLOCK_CTRL:
            logwarn("DMA6_BLOCK_CTRL = %08X", value);
            PS1SYS.dma.block_ctrl[6].raw = value;
            break;
        case DMA6_CHANNEL_CTRL:
            logwarn("DMA6_CHANNEL_CTRL = %08X", value);
            write_dma_channel_ctrl(6, value);
            break;
        default:
            logfatal("Unknown DMA register write: [%08X]=%08X", address, value);
    }
}

u32 dma_register_read(u32 address) {
    switch (address) {
        case DMA_DPCR:
            return PS1SYS.dma.dpcr;
        case DMA_DICR:
            return PS1SYS.dma.dicr;
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
            logfatal("read DMA2_CHANNEL_CTRL");
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
            ps1_create_crash_dump();
            logwarn("read DMA6_CHANNEL_CTRL %08X", PS1SYS.dma.dma_channel_ctrl[6].raw);
            return PS1SYS.dma.dma_channel_ctrl[6].raw;
        default:
            logfatal("Unknown DMA register read: [%08X]", address);
    }
}