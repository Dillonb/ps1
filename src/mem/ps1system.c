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
    memset(&PS1SYS, 0x00, sizeof(PS1SYS));
    load_bios("SCPH1001.BIN");
    cpu_set_pc(0xBFC00000);

    PS1SYS.dma.dpcr = 0x07654321;

//#ifdef PSX_FORCE_TTY /* Patch BIOS to enable TTY output */
    ((uint32_t *)PS1SYS.mem.bios)[0x1bc3] = 0x24010001; /* ADDIU $at, $zero, 0x1 */
    ((uint32_t *)PS1SYS.mem.bios)[0x1bc5] = 0xaf81a9c0; /* SW $at, -0x5640($gp) */
//#endif
}

void ps1_create_crash_dump() {
    log_set_verbosity(LOG_VERBOSITY_DEBUG);
    logdebug("======== CRASH DUMP ========");
    logdebug("PC: 0x%08X", PS1CPU.pc);
    for (int i = 0; i < 32; i++) {
        logdebug("r%-2d $%-4s: 0x%08X", i, register_names[i], PS1CPU.gpr[i]);
    }
    printf("TODO: dump ram to disk\n");
}

_Noreturn void ps1_system_loop() {
    while (1) {
        cpu_step();
    }
}
