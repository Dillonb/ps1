#include "bus.h"
#include <log.h>
#include <mem/addresses.h>
#include <mem/ps1system.h>
#include <mem/mem_util.h>
#include <mem/dma.h>
#include <cpu/cpu.h>

#define CHECK_ISC do { if (PS1CP0.isolate_cache) { return; } } while(0)


INLINE u32 virt_to_phys(u32 virt) {
    switch (virt) {
        case VREGION_KUSEG_VAL:
            return virt - SVREGION_KUSEG_VAL;
        case VREGION_KUSEG_INV:
            logfatal("Invalid region of KUSEG accessed, throw an exception");
            break;
        case VREGION_KSEG0:
            return virt - SVREGION_KSEG0;
        case VREGION_KSEG1:
            return virt - SVREGION_KSEG1;
        case VREGION_KSEG2:
            return virt - SVREGION_KSEG2;
        default:
            logfatal("Unknown region! Virtual address: %08X\n", virt);
    }
}

u8 ps1_read8(u32 virt) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            return PS1SYS.mem.ram[address];
        case REGION_BIOS_ROM: {
            u32 index = address - SREGION_BIOS_ROM;
            if (index < PS1SYS.mem.bios_size) {
                return PS1SYS.mem.bios[index];
            }
            logfatal("Read8 from BIOS: out of range! %08X", address);
        }
        case REGION_DEBUG:
            switch (address) {
                case UART_SRA:
                    return 0x04;
                default:
                    logfatal("ps1_read8 virt [%08X] phys [%08X]", virt, address);
                    break;
            }
            break;
        case REGION_EXP1: return 0xFF; // Expansion port on the back of the console. Ignored.
        default:
            logfatal("ps1_read8 virt [%08X] phys [%08X]", virt, address);
    }
}

u16 ps1_read16(u32 virt) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            return u16_from_byte_array(PS1SYS.mem.ram, address);
        case REGION_SPU:
            logwarn("SPU register read: %08X ignoring.", address);
            return 0x0000;
        case INTC_I_MASK:
            return PS1SYS.i_mask;
    }
    logfatal("ps1_read16 virt %08X phys %08X", virt, address);
}

u32 ps1_read32(u32 virt) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            return u32_from_byte_array(PS1SYS.mem.ram, address);
        case REGION_EXP1: return 0xFFFFFFFF; // Expansion port on the back of the console. Ignored.
        case REGION_BIOS_ROM: {
            u32 index = address - SREGION_BIOS_ROM;
            if (index < PS1SYS.mem.bios_size) {
                return u32_from_byte_array(PS1SYS.mem.bios, index);
            }
            logfatal("Read32 from BIOS: out of range! %08X", address);
        }
        case INTC_I_STAT:
            return PS1SYS.i_stat;
        case INTC_I_MASK:
            return PS1SYS.i_mask;
        case REGION_DMA:
            return dma_register_read(address);
        case GPU_GPUREAD:
            return 0x00000000;
        case GPU_GPUSTAT:
            return 0x1C000000;
            //logfatal("%08X", PS1CPU.pc);
        case REGION_TIMERS:
            return 0x00000000;
        default:
            logfatal("Unknown read32: %08X\n", address);
    }
}

void ps1_write8(u32 virt, u8 value) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            CHECK_ISC;
            PS1SYS.mem.ram[address] = value;
        case REGION_DEBUG:
            switch (address) {
                case UART_THRA:
                    printf("%c", value);
                case EXP2_PSX_POST:
                    loginfo("PSX POST: %02X", value);
                    break;
                default:
                    break;
            }
            break;
        default:
            logfatal("ps1_write8 virt [0x%08X] phys [%08X]=%02X", virt, address, value);
    }
}

void ps1_write16(u32 virt, u16 value) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            CHECK_ISC;
            u16_to_byte_array(PS1SYS.mem.ram, address, value);
        case REGION_SPU:
            logwarn("SPU register write: [%08X]=%04X ignoring.", address, value);
            break;
        case REGION_TIMERS:
            return; // Ignore for now
        case INTC_I_MASK:
            PS1SYS.i_mask = value;
            break;
        default:
            logfatal("Unknown write16: [%08X]=%04X", address, value);
    }
}

void ps1_write32(u32 virt, u32 value) {
    u32 address = virt_to_phys(virt);
    switch (address) {
        case REGION_RAM:
            CHECK_ISC;
            u32_to_byte_array(PS1SYS.mem.ram, address, value);
            break;
        // Expansion Region 1
        // Scratchpad
        // Memory Control 1
        case MEM_CNT_EXP1_BASE:
            logwarn("MEM_CNT_EXP1_BASE = %08X", value);
            break;
        case MEM_CNT_EXP2_BASE:
            logwarn("MEM_CNT_EXP2_BASE = %08X", value);
            break;
        case MEM_CNT_EXP1_SIZE:
            logwarn("MEM_CNT_EXP1_SIZE = %08X", value);
            break;
        case MEM_CNT_EXP3_SIZE:
            logwarn("MEM_CNT_EXP3_SIZE = %08X", value);
            break;
        case MEM_CNT_BIOS_ROM:
            logwarn("MEM_CNT_BIOS_ROM = %08X", value);
            break;
        case MEM_CNT_SPU_SIZE:
            logwarn("MEM_CNT_SPU_SIZE = %08X", value);
            break;
        case MEM_CNT_CDROM_SIZE:
            logwarn("MEM_CNT_CDROM_SIZE = %08X", value);
            break;
        case MEM_CNT_EXP2_SIZE:
            logwarn("MEM_CNT_EXP2_SIZE = %08X", value);
            break;
        case MEM_CNT_COM_DELAY:
            logwarn("MEM_CNT_COM_DELAY = %08X", value);
            break;
        // Peripheral I/O Ports
        // Memory Control 2
        case MEM_CNT_RAM_SIZE:
            logwarn("MEM_CNT_RAM_SIZE = %08X", value);
            break;
        // Interrupt Control
        case INTC_I_STAT:
            PS1SYS.i_stat = value;
            break;
        case INTC_I_MASK:
            PS1SYS.i_mask = value;
            break;

        // DMA Registers
        case REGION_DMA:
            dma_register_write(address, value);
            break;
        case REGION_TIMERS:
            logwarn("Timer register write32, ignoring");
            return; // Ignore for now
        // CDROM Registers
        // GPU Registers
        case GPU_GP0:
            logwarn("GP0 command sent, ignoring");
            break;
        case GPU_GP1:
            logwarn("GP0 command sent, ignoring");
            break;
        // MDEC Registers
        // SPU Voice 0...23 registers
        // SPU Control Registers
        // SPU Reverb Configuration Area
        // SPU Internal Registers
        // Expansion Region 2
        // Expansion Region 3
        // BIOS Region
        // Memory Control 3
        case MEM_CNT_CACHE_CTRL:
            logwarn("MEM_CNT_CACHE_CTRL = %08X", value);
            break;
        default:
            logfatal("ps1_write32 virt [0x%08X] phys [%08X]=%08X", virt, address, value);
    }
}