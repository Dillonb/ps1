#ifndef PS1_DMA_H
#define PS1_DMA_H
#include <util.h>

typedef union dma_channel_ctrl {
    u32 raw;
    struct {
        unsigned direction:1; // 0 = to main ram, 1 = from main ram
        unsigned reverse:1; // 0 = add 4 each transfer, 1 = subtract 4 each transfer
        unsigned:6;
        unsigned chopping:1; // 1 = run CPU during DMA gaps
        unsigned syncmode:2;
        unsigned:2;
        unsigned chopping_dma_window_size:3;
        unsigned:1;
        unsigned chopping_cpu_window_size:3;
        unsigned:1;
        unsigned start_busy:1;
        unsigned:3;
        unsigned start_trigger:1;
        unsigned unknown_maybe_pause:1; // ?
        unsigned unknown:1;
        unsigned:1;
    } PACKED;
} dma_channel_ctrl_t;
ASSERT32(dma_channel_ctrl_t);

typedef struct dma_state {
    u32 dpcr;
    u32 dicr;

    dma_channel_ctrl_t dma_channel_ctrl[7];
} dma_state_t;

void dma_register_write(u32 address, u32 value);
u32 dma_register_read(u32 address);
#endif //PS1_DMA_H
