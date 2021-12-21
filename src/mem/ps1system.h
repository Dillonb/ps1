#ifndef PS1_PS1SYSTEM_H
#define PS1_PS1SYSTEM_H

#include <util.h>
#include <stdlib.h>

void ps1_system_init();

_Noreturn void ps1_system_loop();

typedef struct ps1_mem {
    u8* bios;
    size_t bios_size;
} ps1_mem_t;

typedef struct ps1_system {
    ps1_mem_t mem;
} ps1_system_t;

extern struct ps1_system ps1_system;
#define PS1SYS ps1_system

#endif //PS1_PS1SYSTEM_H
