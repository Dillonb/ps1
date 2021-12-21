#include "ps1system.h"

#include <log.h>
#include <cpu/cpu.h>

ps1_system_t ps1_system;

void load_bios(const char* bios_path) {
    FILE *fp = fopen(bios_path, "rb");

    if (fp == NULL) {
        logfatal("Error opening the BIOS file at %s! Are you sure this is a correct path?", bios_path);
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    u8 *buf = malloc(size);
    fread(buf, size, 1, fp);

    PS1SYS.mem.bios = buf;
    PS1SYS.mem.bios_size = size;

    logalways("Loaded the BIOS, it's %ld bytes.", PS1SYS.mem.bios_size);
}

void ps1_system_init() {
    load_bios("SCPH1001.BIN");
    cpu_set_pc(0xBFC00000);
}

_Noreturn void ps1_system_loop() {
    while (1) {
        cpu_step();
    }
}
