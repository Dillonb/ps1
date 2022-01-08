#include "gpu.h"

#include <log.h>
#include <mem/ps1system.h>

u32 gpu_gpustat() {
    return 0x1C000000;
}

void draw_mode_setting(u32 value) {
    logwarn("Draw mode setting: %08X\n", value);
}

void gpu_gp0_write(u32 value) {
    switch (PS1GPU.gp0_state) {
        case READY: {
            u8 command = (value >> 24) & 0xFF;
            switch (command) {
                case 0x00: // NOP
                    break;
                case 0x01: // Reset command buffer / clear CLUT cache
                    break; // NOP for now
                case 0xA0:
                    PS1GPU.gp0_state = A0_WAITING_FOR_DEST;
                    break;
                case 0xE1:
                    draw_mode_setting(value);
                    break;
                default:
                    logfatal("Unknown GP0 command: %02X", command);
            }
            break;
        }
        case A0_WAITING_FOR_DEST:
            logwarn("Rect CPU to VRAM, dest: %08X", value);
            PS1GPU.gp0_state = A0_WAITING_FOR_SIZE;
            break;
        case A0_WAITING_FOR_SIZE:
            logwarn("Rect CPU to VRAM, size: %08X", value);
            PS1GPU.gp0_state = A0_TRANSFERRING_DATA;
            break;
        case A0_TRANSFERRING_DATA:
            logfatal("A0_TRANSFERRING_DATA");
            break;
    }
}

void display_mode(u32 value) {
    union {
        u32 raw;
        struct {
            unsigned hres:2;
            unsigned vres:1;
            unsigned vmode:1;
            unsigned color_depth:1;
            unsigned v_interlace:1;
            unsigned h_res_2:1;
            unsigned reverse:1;
            unsigned:24;
        };
    } parsed;
    parsed.raw = value;

    int width = 0, height = 0;

}

void display_area(u32 value) {
    PS1GPU.display_start_x = value & 0x3FF;;
    PS1GPU.display_start_y = (value >> 10) & 0x1FF;
}

void gpu_gp1_write(u32 value) {
    u8 command = (value >> 24) & 0xFF;
    switch (command) {
        case 0x00: // Reset GPU
            break;
        case 0x04: // DMA direction
            PS1GPU.dma_direction = value & 3;
            break;
        case 0x05:
            display_area(value);
            break;
        case 0x06:
            //horiz_display_range(value);
            break;
        case 0x07:
            //vert_display_range(value);
            break;
        case 0x08:
            display_mode(value);
            break;
        default:
            logfatal("Unknown GP1 command: %02X", command);
    }
}