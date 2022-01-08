#ifndef PS1_GPU_H
#define PS1_GPU_H
#include <util.h>

typedef enum ps1_gpu_dma_direction {
    OFF,
    FIFO,
    CPUtoGP0,
    GPUREADtoCPU
} ps1_gpu_dma_direction_t;

typedef enum ps1_gp0_state {
    READY,

    // Copy rectangle - CPU to VRAM
    A0_WAITING_FOR_DEST,
    A0_WAITING_FOR_SIZE,
    A0_TRANSFERRING_DATA,


} ps1_gp0_state_t;

typedef struct ps1_gpu {
    ps1_gpu_dma_direction_t dma_direction;

    ps1_gp0_state_t gp0_state;

    int display_start_x;
    int display_start_y;

} ps1_gpu_t;

u32 gpu_gpustat();
void gpu_gp0_write(u32 value);
void gpu_gp1_write(u32 value);
#endif //PS1_GPU_H
