#include "mips_instructions.h"
#include "cpu_register_access.h"

#include <log.h>
#include <mem/bus.h>

void check_s32_add_overflow(s32 addend1, s32 addend2, s32 result) {
    if (addend1 > 0 && addend2 > 0) {
        if (result < 0) {
            logfatal("Integer overflow exception");
        }
    } else if (addend1 < 0 && addend2 < 0) {
        if (result > 0) {
            logfatal("Integer overflow exception");
        }
    }
}

MIPS_INSTR(mips_nop) {}

MIPS_INSTR(mips_addi) {
    s32 reg_addend = get_register(instruction.i.rs);
    s16 imm_addend = instruction.i.immediate;
    s32 result = imm_addend + reg_addend;
    check_s32_add_overflow(imm_addend, reg_addend, result);
    set_register(instruction.i.rt, result);
}

MIPS_INSTR(mips_addiu) {
    u32 reg_addend = get_register(instruction.i.rs);
    s16 addend = instruction.i.immediate;
    s32 result = reg_addend + addend;

    set_register(instruction.i.rt, result);
}

MIPS_INSTR(mips_andi) {
    u32 immediate = instruction.i.immediate;
    u32 result = immediate & get_register(instruction.i.rs);
    set_register(instruction.i.rt, result);
}

MIPS_INSTR(mips_beq) {
    conditional_branch(instruction.i.immediate, get_register(instruction.i.rs) == get_register(instruction.i.rt));
}

MIPS_INSTR(mips_bgtz) {
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate,  reg > 0);
}

MIPS_INSTR(mips_blez) {
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate, reg <= 0);
}

MIPS_INSTR(mips_bne) {
    conditional_branch(instruction.i.immediate, get_register(instruction.i.rs) != get_register(instruction.i.rt));
}

MIPS_INSTR(mips_cache) {
    return; // No need to emulate the cache. Might be fun to do someday for accuracy.
}

MIPS_INSTR(mips_j) {
    u32 target = instruction.j.target;
    target <<= 2;
    target |= ((PS1CPU.pc - 4) & 0xFFFFFFFFF0000000); // PC is 4 ahead

    branch_abs(target);
}

MIPS_INSTR(mips_jal) {
    link(CPU_REG_LR);

    u32 target = instruction.j.target;
    target <<= 2;
    target |= ((PS1CPU.pc - 4) & 0xFFFFFFFFF0000000); // PC is 4 ahead

    branch_abs(target);
}

MIPS_INSTR(mips_slti) {
    s16 immediate = instruction.i.immediate;
    logtrace("Set if %ld < %d", get_register(instruction.i.rs), immediate);
    s32 reg = get_register(instruction.i.rs);
    if (reg < immediate) {
        set_register(instruction.i.rt, 1);
    } else {
        set_register(instruction.i.rt, 0);
    }
}

MIPS_INSTR(mips_sltiu) {
    s16 immediate = instruction.i.immediate;
    logtrace("Set if %ld < %d", get_register(instruction.i.rs), immediate);
    if (get_register(instruction.i.rs) < immediate) {
        set_register(instruction.i.rt, 1);
    } else {
        set_register(instruction.i.rt, 0);
    }
}

MIPS_INSTR(mips_mfc0) {
    s32 value = get_cp0_register_word(instruction.r.rd);
    set_register(instruction.r.rt, value);
}

MIPS_INSTR(mips_mtc0) {
    u32 value = get_register(instruction.r.rt);
    set_cp0_register(instruction.r.rd, value);
}

MIPS_INSTR(mips_lui) {
    // Done this way to avoid the undefined behavior of left shifting a signed integer
    // Should compile to a left shift by 16.
    s32 value = (s16)instruction.i.immediate;
    value *= 65536;

    set_register(instruction.i.rt, value);
}

MIPS_INSTR(mips_lbu) {
    s16 offset = instruction.i.immediate;
    logtrace("LBU offset: %d", offset);
    u32 address = get_register(instruction.i.rs) + offset;
    u8 value = ps1_read8(address);
    set_register(instruction.i.rt, value); // zero extend
}

MIPS_INSTR(mips_lhu) {
    s16 offset = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs) + offset;
    if ((address & 0b1) > 0) {
        logfatal("TODO: throw an 'address error' exception! Tried to load from unaligned address 0x%08X", address);
    }

    u16 value = ps1_read16(address);
    set_register(instruction.i.rt, value); // zero extend
}

MIPS_INSTR(mips_lh) {
    s16 offset = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs) + offset;
    s16 value   = ps1_read16(address);
    if ((address & 0b1) > 0) {
        logfatal("TODO: throw an 'address error' exception! Tried to load from unaligned address 0x%08X", address);
    }

    set_register(instruction.i.rt, value);
}

MIPS_INSTR(mips_lw) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs) + offset;
    if ((address & 0b11) > 0) {
        logfatal("TODO: throw an 'address error' exception! Tried to load from unaligned address 0x%08X", address);
    }

    s32 value = ps1_read32(address);
    set_register(instruction.i.rt, value);
}

MIPS_INSTR(mips_lwu) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs) + offset;
    if ((address & 0b11) > 0) {
        logfatal("TODO: throw an 'address error' exception! Tried to load from unaligned address 0x%08X", address);
    }

    u32 value = ps1_read32(address);
    set_register(instruction.i.rt, value);
}

MIPS_INSTR(mips_sb) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs);
    address += offset;
    u8 value = get_register(instruction.i.rt) & 0xFF;
    ps1_write8(address, value);
}

MIPS_INSTR(mips_sh) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs);
    address += offset;
    u16 value = get_register(instruction.i.rt);
    ps1_write16(address, value);
}

MIPS_INSTR(mips_sw) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs);
    address += offset;
    ps1_write32(address, get_register(instruction.i.rt));
}

MIPS_INSTR(mips_ori) {
    set_register(instruction.i.rt, instruction.i.immediate | get_register(instruction.i.rs));
}

MIPS_INSTR(mips_xori) {
    set_register(instruction.i.rt, instruction.i.immediate ^ get_register(instruction.i.rs));
}

MIPS_INSTR(mips_lb) {
    s16 offset  = instruction.i.immediate;
    u32 address = get_register(instruction.i.rs) + offset;
    s8 value   = ps1_read8(address);

    set_register(instruction.i.rt, value);
}

MIPS_INSTR(mips_lwl) {
    s16 offset  = instruction.fi.offset;
    u32 address = get_register(instruction.fi.base) + offset;


    u32 shift = 8 * ((address ^ 0) & 3);
    u32 mask = 0xFFFFFFFF << shift;
    u32 data = ps1_read32(address & ~3);
    s32 result = (get_register(instruction.i.rt) & ~mask) | data << shift;
    set_register(instruction.i.rt, result);
}

MIPS_INSTR(mips_lwr) {
    s16 offset  = instruction.fi.offset;
    u32 address = get_register(instruction.fi.base) + offset;

    u32 shift = 8 * ((address ^ 3) & 3);

    u32 mask = 0xFFFFFFFF >> shift;
    u32 data = ps1_read32(address & ~3);
    s32 result = (get_register(instruction.i.rt) & ~mask) | data >> shift;
    set_register(instruction.i.rt, result);
}

MIPS_INSTR(mips_swl) {
    s16 offset  = instruction.fi.offset;
    u32 address = get_register(instruction.fi.base) + offset;
    u32 shift = 8 * ((address ^ 0) & 3);
    u32 mask = 0xFFFFFFFF >> shift;
    u32 data = ps1_read32(address & ~3);
    u32 oldreg = get_register(instruction.i.rt);
    ps1_write32(address & ~3, (data & ~mask) | (oldreg >> shift));
}

MIPS_INSTR(mips_swr) {
    s16 offset  = instruction.fi.offset;
    u32 address = get_register(instruction.fi.base) + offset;
    u32 physical;
    u32 shift = 8 * ((address ^ 3) & 3);
    u32 mask = 0xFFFFFFFF << shift;
    u32 data = ps1_read32(physical & ~3);
    u32 oldreg = get_register(instruction.i.rt);
    ps1_write32(physical & ~3, (data & ~mask) | oldreg << shift);
}

MIPS_INSTR(mips_spc_sll) {
    s32 result = get_register(instruction.r.rt) << instruction.r.sa;
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_srl) {
    u32 value = get_register(instruction.r.rt);
    s32 result = value >> instruction.r.sa;
    set_register(instruction.r.rd,  result);
}

MIPS_INSTR(mips_spc_sra) {
    s32 value = get_register(instruction.r.rt);
    s32 result = value >> instruction.r.sa;
    set_register(instruction.r.rd,  result);
}

MIPS_INSTR(mips_spc_srav) {
    s32 value = get_register(instruction.r.rt);
    s32 result = value >> (get_register(instruction.r.rs) & 0b11111);
    set_register(instruction.r.rd,  result);
}

MIPS_INSTR(mips_spc_sllv) {
    u32 value = get_register(instruction.r.rt);
    s32 result = value << (get_register(instruction.r.rs) & 0b11111);
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_srlv) {
    u32 value = get_register(instruction.r.rt);
    s32 result = value >> (get_register(instruction.r.rs) & 0b11111);
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_jr) {
    branch_abs(get_register(instruction.r.rs));
}

MIPS_INSTR(mips_spc_jalr) {
    link(instruction.r.rd);
    branch_abs(get_register(instruction.r.rs));
}

MIPS_INSTR(mips_spc_syscall) {
    cpu_handle_exception(PS1CPU.prev_pc, EXCEPTION_SYSCALL, -1);
}

MIPS_INSTR(mips_spc_mfhi) {
    set_register(instruction.r.rd, PS1CPU.mult_hi);
}

MIPS_INSTR(mips_spc_mthi) {
    PS1CPU.mult_hi = get_register(instruction.r.rs);
}

MIPS_INSTR(mips_spc_mflo) {
    set_register(instruction.r.rd, PS1CPU.mult_lo);
}

MIPS_INSTR(mips_spc_mtlo) {
    PS1CPU.mult_lo = get_register(instruction.r.rs);
}

MIPS_INSTR(mips_spc_mult) {
    s32 multiplicand_1 = get_register(instruction.r.rs);
    s32 multiplicand_2 = get_register(instruction.r.rt);

    s64 dmultiplicand_1 = multiplicand_1;
    s64 dmultiplicand_2 = multiplicand_2;

    s64 result = dmultiplicand_1 * dmultiplicand_2;

    u32 result_lower = result         & 0xFFFFFFFF;
    u32 result_upper = ((u64)result >> 32) & 0xFFFFFFFF;

    PS1CPU.mult_lo = result_lower;
    PS1CPU.mult_hi = result_upper;
}

MIPS_INSTR(mips_spc_multu) {
    u64 multiplicand_1 = get_register(instruction.r.rs);
    u64 multiplicand_2 = get_register(instruction.r.rt);

    u64 result = multiplicand_1 * multiplicand_2;

    s32 result_lower = result         & 0xFFFFFFFF;
    s32 result_upper = (result >> 32) & 0xFFFFFFFF;

    PS1CPU.mult_lo = result_lower;
    PS1CPU.mult_hi = result_upper;
}

MIPS_INSTR(mips_spc_div) {
    s32 dividend = get_register(instruction.r.rs);
    s32 divisor  = get_register(instruction.r.rt);

    if (divisor == 0) {
        logwarn("Divide by zero");
        PS1CPU.mult_hi = dividend;
        if (dividend >= 0) {
            PS1CPU.mult_lo = (s32)-1;
        } else {
            PS1CPU.mult_lo = (s32)1;
        }
    } else {
        s32 quotient  = dividend / divisor;
        s32 remainder = dividend % divisor;

        PS1CPU.mult_lo = quotient;
        PS1CPU.mult_hi = remainder;
    }

}

MIPS_INSTR(mips_spc_divu) {
    u32 dividend = get_register(instruction.r.rs);
    u32 divisor  = get_register(instruction.r.rt);

    if (divisor == 0) {
        PS1CPU.mult_lo = 0xFFFFFFFF;
        PS1CPU.mult_hi = dividend;
    } else {
        s32 quotient  = dividend / divisor;
        s32 remainder = dividend % divisor;

        PS1CPU.mult_lo = quotient;
        PS1CPU.mult_hi = remainder;
    }
}

MIPS_INSTR(mips_spc_add) {
    s32 addend1 = get_register(instruction.r.rs);
    s32 addend2 = get_register(instruction.r.rt);

    s32 result = addend1 + addend2;
    check_s32_add_overflow(addend1, addend2, result);

    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_addu) {
    u32 rs = get_register(instruction.r.rs);
    u32 rt = get_register(instruction.r.rt);
    u32 result = rs + rt;
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_and) {
    u32 result = get_register(instruction.r.rs) & get_register(instruction.r.rt);
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_nor) {
    u32 result = ~(get_register(instruction.r.rs) | get_register(instruction.r.rt));
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_sub) {
    s32 operand1 = get_register(instruction.r.rs);
    s32 operand2 = get_register(instruction.r.rt);

    s32 result = operand1 - operand2;
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_subu) {
    u32 operand1 = get_register(instruction.r.rs);
    u32 operand2 = get_register(instruction.r.rt);

    u32 result = operand1 - operand2;
    set_register(instruction.r.rd, result);
}

MIPS_INSTR(mips_spc_or) {
    set_register(instruction.r.rd, get_register(instruction.r.rs) | get_register(instruction.r.rt));
}

MIPS_INSTR(mips_spc_xor) {
    set_register(instruction.r.rd, get_register(instruction.r.rs) ^ get_register(instruction.r.rt));
}

MIPS_INSTR(mips_spc_slt) {
    s32 op1 = get_register(instruction.r.rs);
    s32 op2 = get_register(instruction.r.rt);

    // RS - RT
    s32 result = op1 - op2;
    // if RS is LESS than RT
    // aka, if result is negative

    logtrace("Set if %ld < %ld", op1, op2);
    if (result < 0) {
        set_register(instruction.r.rd, 1);
    } else {
        set_register(instruction.r.rd, 0);
    }
}

MIPS_INSTR(mips_spc_sltu) {
    u32 op1 = get_register(instruction.r.rs);
    u32 op2 = get_register(instruction.r.rt);

    logtrace("Set if %lu < %lu", op1, op2);
    if (op1 < op2) {
        set_register(instruction.r.rd, 1);
    } else {
        set_register(instruction.r.rd, 0);
    }
}

MIPS_INSTR(mips_spc_teq) {
    u32 rs = get_register(instruction.r.rs);
    u32 rt = get_register(instruction.r.rt);

    if (rs == rt) {
        cpu_handle_exception(PS1CPU.prev_pc, EXCEPTION_TRAP, -1);
    }
}

MIPS_INSTR(mips_spc_break) {
    cpu_handle_exception(PS1CPU.prev_pc, EXCEPTION_BREAKPOINT, -1);
}

MIPS_INSTR(mips_spc_tne) {
    u32 rs = get_register(instruction.r.rs);
    u32 rt = get_register(instruction.r.rt);

    if (rs != rt) {
        cpu_handle_exception(PS1CPU.prev_pc, EXCEPTION_TRAP, -1);
    }
}

MIPS_INSTR(mips_ri_bltz) {
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate, reg < 0);
}

MIPS_INSTR(mips_ri_bgez) {
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate, reg >= 0);
}

MIPS_INSTR(mips_ri_bltzal) {
    link(CPU_REG_LR);
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate, reg < 0);
}

MIPS_INSTR(mips_ri_bgezal) {
    link(CPU_REG_LR);
    s32 reg = get_register(instruction.i.rs);
    conditional_branch(instruction.i.immediate, reg >= 0);
}

MIPS_INSTR(mips_rfe) {
    PS1CP0.status.ie_ku >>= 2;
}
