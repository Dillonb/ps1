#ifndef PS1_BUS_H
#define PS1_BUS_H

#include <util.h>

u8 ps1_read8(u32 address);
u16 ps1_read16(u32 address);
u32 ps1_read32(u32 address);

void ps1_write8(u32 address, u8 value);
void ps1_write16(u32 address, u16 value);
void ps1_write32(u32 address, u32 value);

#endif //PS1_BUS_H
