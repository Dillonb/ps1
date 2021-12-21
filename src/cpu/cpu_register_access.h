#ifndef N64_R3000A_REGISTER_ACCESS_H
#define N64_R3000A_REGISTER_ACCESS_H

#include <log.h>

#include "cpu.h"

INLINE void set_register(unsigned int r, u32 value) {
    logtrace("Setting $%s (r%d) to [0x%08X]", register_names[r], r, value);
    PS1CPU.gpr[r] = value;
    PS1CPU.gpr[0] = 0;
}

INLINE s32 get_register(unsigned int r) {
    s32 value = PS1CPU.gpr[r];
    logtrace("Reading $%s (r%d): 0x%08X", register_names[r], r, value);
    return value;
}

INLINE void log_status(cp0_status_t status) {
    loginfo("    CP0 status: ie:  %d", status.ie);
    loginfo("    CP0 status: exl: %d", status.exl);
    loginfo("    CP0 status: erl: %d", status.erl);
    loginfo("    CP0 status: ksu: %d", status.ksu);
    loginfo("    CP0 status: ux:  %d", status.ux);
    loginfo("    CP0 status: sx:  %d", status.sx);
    loginfo("    CP0 status: kx:  %d", status.kx);
    loginfo("    CP0 status: im:  %d", status.im);
    loginfo("    CP0 status: ds:  %d", status.ds);
    loginfo("    CP0 status: re:  %d", status.re);
    loginfo("    CP0 status: fr:  %d", status.fr);
    loginfo("    CP0 status: rp:  %d", status.rp);
    loginfo("    CP0 status: cu0: %d", status.cu0);
    loginfo("    CP0 status: cu1: %d", status.cu1);
    loginfo("    CP0 status: cu2: %d", status.cu2);
    loginfo("    CP0 status: cu3: %d", status.cu3);
}

INLINE void set_cp0_register(int r, u32 value) {
    switch (r) {
        case CP0_REG_INDEX:
        case CP0_REG_RANDOM:
        case CP0_REG_COUNT:
        case CP0_REG_CAUSE:
        case CP0_REG_TAGLO:
        case CP0_REG_TAGHI:
        case CP0_REG_COMPARE:
        case CP0_REG_STATUS:
        case CP0_REG_ENTRYLO0:
        case CP0_REG_ENTRYLO1:
        case CP0_REG_7:
        case CP0_REG_ENTRYHI:
        case CP0_REG_PAGEMASK:
        case CP0_REG_EPC:
        case CP0_REG_CONFIG:
        case CP0_REG_WATCHLO:
        case CP0_REG_WATCHHI:
        case CP0_REG_WIRED:
        case CP0_REG_CONTEXT:
        case CP0_REG_LLADDR:
        case CP0_REG_ERR_EPC:
        default:
            logfatal("Unsupported CP0 $%s (%d) set: 0x%08X", cp0_register_names[r], r, value);
    }
    /*
    switch (r) {
        case CP0_REG_INDEX:
            PS1CPU.cp0.index = value;
            break;
        case CP0_REG_RANDOM:
            break;
        case CP0_REG_COUNT:
            PS1CPU.cp0.count = (dword)value << 1;
            break;
        case CP0_REG_CAUSE: {
            cp0_cause_t newcause;
            newcause.raw = value;
            PS1CPU.cp0.cause.ip0 = newcause.ip0;
            PS1CPU.cp0.cause.ip1 = newcause.ip1;
            break;
        }
        case CP0_REG_TAGLO: // Used for the cache, which is unimplemented.
            PS1CPU.cp0.tag_lo = value;
            break;
        case CP0_REG_TAGHI: // Used for the cache, which is unimplemented.
            PS1CPU.cp0.tag_hi = value;
            break;
        case CP0_REG_COMPARE:
            loginfo("$Compare written with 0x%08X (count is now 0x%08lX)", value, PS1CPU.cp0.count);
            PS1CPU.cp0.cause.ip7 = false;
            PS1CPU.cp0.compare = value;
            break;
        case CP0_REG_STATUS: {
            PS1CPU.cp0.status.raw &= ~CP0_STATUS_WRITE_MASK;
            PS1CPU.cp0.status.raw |= value & CP0_STATUS_WRITE_MASK;

            unimplemented(PS1CPU.cp0.status.re, "Reverse endian bit set in CP0 (this probably ");
            // TODO: make sure to fix the CPU_MODE_SUPERVISOR and CPU_MODE_USER constants when this assertion gets hit
            unimplemented(PS1CPU.cp0.status.ksu, "KSU != 0, leaving kernel mode!");

            cp0_status_updated();
            log_status(PS1CPU.cp0.status);

            r3000a_interrupt_update();
            break;
        }
        case CP0_REG_ENTRYLO0:
            PS1CPU.cp0.entry_lo0.raw = value;
            break;
        case CP0_REG_ENTRYLO1:
            PS1CPU.cp0.entry_lo1.raw = value;
            break;
        case 7:
            logfatal("CP0 Reg 7 write?");
            break;
        case CP0_REG_ENTRYHI:
            PS1CPU.cp0.entry_hi.raw = value;
            break;
        case CP0_REG_PAGEMASK:
            PS1CPU.cp0.page_mask.raw = value;
            break;
        case CP0_REG_EPC:
            PS1CPU.cp0.EPC = (sdword)((sword)value);
            break;
        case CP0_REG_CONFIG:
            PS1CPU.cp0.config &= ~CP0_CONFIG_WRITE_MASK;
            PS1CPU.cp0.config |= value & CP0_CONFIG_WRITE_MASK;
            break;
        case CP0_REG_WATCHLO:
            PS1CPU.cp0.watch_lo.raw = value;
            unimplemented(PS1CPU.cp0.watch_lo.r, "Read exception enabled in CP0 watch_lo!");
            unimplemented(PS1CPU.cp0.watch_lo.w, "Write exception enabled in CP0 watch_lo!");
            break;
        case CP0_REG_WATCHHI:
            PS1CPU.cp0.watch_hi = value;
            break;
        case CP0_REG_WIRED:
            PS1CPU.cp0.wired = value;
            break;
        case CP0_REG_CONTEXT:
            unimplemented(PS1CPU.cp0.is_64bit_addressing, "Context written as word in 64 bit mode");
            unimplemented(value != 0, "cp0 context written with non-zero value");
            PS1CPU.cp0.context.raw = value;
            break;
        case CP0_REG_LLADDR:
            PS1CPU.cp0.lladdr = value;
            break;
        case CP0_REG_ERR_EPC:
            N64CP0.error_epc = (sdword)((sword)value);
            break;
        default:
            logfatal("Unsupported CP0 $%s (%d) set: 0x%08X", cp0_register_names[r], r, value);
    }
     */

    loginfo("CP0 $%s = 0x%08X", cp0_register_names[r], value);
}

INLINE u32 get_cp0_count() {
    u64 shifted = PS1CPU.cp0.count >> 1;
    return (u32)shifted;
}

INLINE u32 get_cp0_wired() {
    logfatal("get cp0 wired");
    //return PS1CP0.wired & 0b111111;
}

INLINE u32 get_cp0_random() {
    logfatal("Get cp0 random");
    /*
    int upper = 32 - get_cp0_wired();
    int val = rand() % upper;
    val += get_cp0_wired();

    return val;
     */
}

INLINE u32 get_cp0_register_word(int r) {
    switch (r) {
        /*
        case CP0_REG_INDEX:
        case CP0_REG_RANDOM:
        case CP0_REG_ENTRYLO0:
        case CP0_REG_ENTRYLO1:
        case CP0_REG_CONTEXT:
        case CP0_REG_PAGEMASK:
        case CP0_REG_WIRED:
        case CP0_REG_7:
        case CP0_REG_BADVADDR:
        case CP0_REG_COUNT:
        case CP0_REG_ENTRYHI:
        case CP0_REG_COMPARE:
        case CP0_REG_STATUS:
        case CP0_REG_CAUSE:
        case CP0_REG_EPC:
        case CP0_REG_PRID:
        case CP0_REG_CONFIG:
        case CP0_REG_LLADDR:
        case CP0_REG_WATCHLO:
        case CP0_REG_WATCHHI:
        case CP0_REG_XCONTEXT:
        case CP0_REG_21:
        case CP0_REG_22:
        case CP0_REG_23:
        case CP0_REG_24:
        case CP0_REG_25:
        case CP0_REG_PARITYER:
        case CP0_REG_CACHEER:
        case CP0_REG_TAGLO:
        case CP0_REG_TAGHI:
        case CP0_REG_ERR_EPC:
        case CP0_REG_31:
         */
        default:
            logfatal("Unsupported CP0 $%s (%d) read", cp0_register_names[r], r);
    }
    /*
    switch (r) {
        case CP0_REG_INDEX:
            return PS1CPU.cp0.index & 0x8000003F;
        case CP0_REG_RANDOM:
            logalways("WARNING! Stubbed read from Random!");
            return 1;
        case CP0_REG_ENTRYLO0:
            return PS1CPU.cp0.entry_lo0.raw;
        case CP0_REG_ENTRYLO1:
            return PS1CPU.cp0.entry_lo1.raw;
        case CP0_REG_CONTEXT:
            return PS1CPU.cp0.context.raw;
        case CP0_REG_PAGEMASK:
            return PS1CPU.cp0.page_mask.raw;
        case CP0_REG_WIRED:
            return PS1CPU.cp0.wired;
        case CP0_REG_7:
            return PS1CPU.cp0.r7;
        case CP0_REG_BADVADDR:
            return PS1CPU.cp0.bad_vaddr;
        case CP0_REG_COUNT:
            return get_cp0_count();
        case CP0_REG_ENTRYHI:
            unimplemented(PS1CPU.cp0.is_64bit_addressing, "entryhi read as word in 64 bit mode");
            return PS1CPU.cp0.entry_hi.raw;
        case CP0_REG_COMPARE:
            return PS1CPU.cp0.compare;
        case CP0_REG_STATUS:
            return PS1CPU.cp0.status.raw;
        case CP0_REG_CAUSE:
            return PS1CPU.cp0.cause.raw;
        case CP0_REG_EPC:
            return PS1CPU.cp0.EPC & 0xFFFFFFFF;
        case CP0_REG_PRID:
            return PS1CPU.cp0.PRId;
        case CP0_REG_CONFIG:
            return PS1CPU.cp0.config;
        case CP0_REG_LLADDR:
            return PS1CPU.cp0.lladdr;
        case CP0_REG_WATCHLO:
            return PS1CPU.cp0.watch_lo.raw;
        case CP0_REG_WATCHHI:
            return PS1CPU.cp0.watch_hi;
        case CP0_REG_XCONTEXT:
            return PS1CPU.cp0.x_context.raw;
        case CP0_REG_21:
            return PS1CPU.cp0.r21;
        case CP0_REG_22:
            return PS1CPU.cp0.r22;
        case CP0_REG_23:
            return PS1CPU.cp0.r23;
        case CP0_REG_24:
            return PS1CPU.cp0.r24;
        case CP0_REG_25:
            return PS1CPU.cp0.r25;
        case CP0_REG_PARITYER:
            return PS1CPU.cp0.parity_error;
        case CP0_REG_CACHEER:
            return PS1CPU.cp0.cache_error;
        case CP0_REG_TAGLO:
            return PS1CPU.cp0.tag_lo;
        case CP0_REG_TAGHI:
            return PS1CPU.cp0.tag_hi;
        case CP0_REG_ERR_EPC:
            return PS1CPU.cp0.error_epc & 0xFFFFFFFF;
        case CP0_REG_31:
            return PS1CPU.cp0.r31;
        default:
            logfatal("Unsupported CP0 $%s (%d) read", cp0_register_names[r], r);
    }
     */
}

INLINE void link(int reg) {
    u32 pc = PS1CPU.pc + 4;
    set_register(reg, pc); // Skips the instruction in the delay slot on return
}

INLINE void branch_abs(u32 address) {
    PS1CPU.next_pc = address;
    PS1CPU.branch = true;
}

INLINE void branch_offset(s16 offset) {
    s32 soffset = offset;
    soffset *= 4;
    // This is taking advantage of the fact that we add 4 to the PC after each instruction.
    // Due to the compiler expecting pipelining, the address we get here will be 4 _too early_

    branch_abs(PS1CPU.pc + soffset);
}

INLINE void conditional_branch_likely(s16 offset, bool condition) {
    if (condition) {
        branch_offset(offset);
    } else {
        // Skip instruction in delay slot
        cpu_set_pc(PS1CPU.pc + 4);
    }
}

INLINE void conditional_branch(s16 offset, bool condition) {
    if (condition) {
        branch_offset(offset);
    }
}

#endif //N64_R3000A_REGISTER_ACCESS_H
