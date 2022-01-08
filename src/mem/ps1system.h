#ifndef PS1_PS1SYSTEM_H
#define PS1_PS1SYSTEM_H

#include <util.h>
#include <stdlib.h>
#include <gpu/gpu.h>
#include <mem/dma.h>

void ps1_system_init();
void ps1_create_crash_dump();

_Noreturn void ps1_system_loop();

typedef struct ps1_mem {
    u8* bios;
    size_t bios_size;

    u8 ram[0x200000];
} ps1_mem_t;

typedef struct ps1_system {
    ps1_mem_t mem;

    u16 i_mask;
    u16 i_stat;
    ps1_gpu_t gpu;
    dma_state_t dma;
} ps1_system_t;

extern struct ps1_system ps1_system;
#define PS1SYS ps1_system

#define PS1GPU ps1_system.gpu

#endif //PS1_PS1SYSTEM_H
