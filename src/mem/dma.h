#ifndef PS1_DMA_H
#define PS1_DMA_H
#include <util.h>
void dma_register_write(u32 address, u32 value);
u32 dma_register_read(u32 address);
#endif //PS1_DMA_H
