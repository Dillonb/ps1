#include "bus.h"
#include <log.h>
#include <mem/addresses.h>
#include <mem/ps1system.h>
#include <mem/mem_util.h>

INLINE u32 virt_to_phys(u32 virt) {
    switch (virt) {
        case VREGION_KUSEG:
            logfatal("VREGION_KUSEG");
        case VREGION_KSEG0:
            return virt - SVREGION_KSEG0;
        case VREGION_KSEG1:
            return virt - SVREGION_KSEG1;
        case VREGION_KSSEG:
            logfatal("VREGION_KSSEG");
        case VREGION_KSEG3:
            logfatal("VREGION_KSEG3");
        default:
            logfatal("Unknown region! Virtual address: %08X\n", virt);
    }
}

u8 ps1_read8(u32 address) {
    logfatal("ps1_read8");
}

u16 ps1_read16(u32 address) {
    logfatal("ps1_read16");
}

u32 ps1_read32(u32 address) {
    u32 phys = virt_to_phys(address);
    switch (phys) {
        case REGION_BIOS_ROM: {
            u32 index = phys - SREGION_BIOS_ROM;
            if (index < PS1SYS.mem.bios_size) {
                return u32_from_byte_array(PS1SYS.mem.bios, index);
                logfatal("bios");
            }
        }
        default:
            logfatal("Unknown address on read32: %08X\n", phys);
    }
}

void ps1_write8(u32 address, u8 value) {
    logfatal("ps1_write8");
}

void ps1_write16(u32 address, u16 value) {
    logfatal("ps1_write16");
}

void ps1_write32(u32 address, u32 value) {
    logfatal("ps1_write32 [%08X]=%08X", address, value);
}