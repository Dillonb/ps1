#include "cpu.h"

#include <util.h>
#include <log.h>
#include "disassemble.h"
#include "mips_instructions.h"
#include <mem/bus.h>

const char* register_names[] = {
        "zero", // 0
        "at",   // 1
        "v0",   // 2
        "v1",   // 3
        "a0",   // 4
        "a1",   // 5
        "a2",   // 6
        "a3",   // 7
        "t0",   // 8
        "t1",   // 9
        "t2",   // 10
        "t3",   // 11
        "t4",   // 12
        "t5",   // 13
        "t6",   // 14
        "t7",   // 15
        "s0",   // 16
        "s1",   // 17
        "s2",   // 18
        "s3",   // 19
        "s4",   // 20
        "s5",   // 21
        "s6",   // 22
        "s7",   // 23
        "t8",   // 24
        "t9",   // 25
        "k0",   // 26
        "k1",   // 27
        "gp",   // 28
        "sp",   // 29
        "s8",   // 30
        "ra"    // 31
};

const char* cp0_register_names[] = {
        "0", "1", "2", "BPC", "4", "BDA", "JUMPDEST", "DCIC", "BadVAddr", "BDAM", "10",
        "BPCM", "SR", "CAUSE", "EPC", "PRId", "16", "17", "18", "19", "20", "21", "22",
        "23", "24", "25", "26", "27", "28", "29", "30", "31"
};

r3000a_t ps1cpu;

void cpu_handle_exception(u32 pc, u32 code, s32 coprocessor_error) {
    loginfo("Exception thrown! Code: %d Coprocessor: %d", code, coprocessor_error);
    // In a branch delay slot, set EPC to the branch PRECEDING the slot.
    // This is so the exception handler can re-execute the branch on return.
    if (PS1CPU.branch) {
        PS1CPU.cp0.cause.branch_delay = true;
        PS1CPU.branch = false;
        pc -= 4;
    } else {
        PS1CPU.cp0.cause.branch_delay = false;
    }

    PS1CPU.cp0.EPC = pc;

    PS1CP0.status.ie_ku <<= 2;

    PS1CPU.cp0.cause.exception_code = code;
    if (coprocessor_error > 0) {
        PS1CPU.cp0.cause.coprocessor_error = coprocessor_error;
    }

    if (PS1CPU.cp0.status.bev) {
        switch (code) {
            case EXCEPTION_COPROCESSOR_UNUSABLE:
                logfatal("Cop unusable, the PC below is wrong. See page 181 in the manual.");
                cpu_set_pc(0x80000180);
                break;
            default:
                logfatal("Unknown exception %d with BEV! See page 181 in the manual.", code);
        }
    } else {
        switch (code) {
            case EXCEPTION_INTERRUPT:
            case EXCEPTION_COPROCESSOR_UNUSABLE:
            case EXCEPTION_TRAP:
            case EXCEPTION_BREAKPOINT:
            case EXCEPTION_SYSCALL:
                cpu_set_pc(0x80000080);
                break;
            default:
                logfatal("Unknown exception %d without BEV! See page 181 in the manual.", code);
        }
    }
    cp0_status_updated();
    PS1CPU.exception = true;
}

INLINE mipsinstr_handler_t r3000a_cp0_decode(u32 pc, mips_instruction_t instr) {
    if (instr.last11 == 0) {
        switch (instr.r.rs) {
            case COP_MF:
                return mips_mfc0;
            case COP_MT: // Last 11 bits are 0
                return mips_mtc0;
            default: {
                char buf[50];
                disassemble(pc, instr.raw, buf, 50);
                logfatal("other/unknown MIPS CP0 0x%08X with rs: %d%d%d%d%d [%s]", instr.raw,
                         instr.rs0, instr.rs1, instr.rs2, instr.rs3, instr.rs4, buf);
            }
        }
    } else {
        switch (instr.fr.funct) {
            case COP_FUNCT_WAIT:
                return mips_nop;
            case COP_FUNCT_RFE:
                return mips_rfe;
            default: {
                char buf[50];
                disassemble(pc, instr.raw, buf, 50);
                logfatal("other/unknown MIPS CP0 0x%08X with FUNCT: %d%d%d%d%d%d [%s]", instr.raw,
                         instr.funct0, instr.funct1, instr.funct2, instr.funct3, instr.funct4, instr.funct5, buf);
            }
        }
    }
}

INLINE mipsinstr_handler_t r3000a_special_decode(u32 pc, mips_instruction_t instr) {
    switch (instr.r.funct) {
        case FUNCT_SLL:     return mips_spc_sll;
        case FUNCT_SRL:     return mips_spc_srl;
        case FUNCT_SRA:     return mips_spc_sra;
        case FUNCT_SRAV:    return mips_spc_srav;
        case FUNCT_SLLV:    return mips_spc_sllv;
        case FUNCT_SRLV:    return mips_spc_srlv;
        case FUNCT_JR:      return mips_spc_jr;
        case FUNCT_JALR:    return mips_spc_jalr;
        case FUNCT_SYSCALL: return mips_spc_syscall;
        case FUNCT_MFHI:    return mips_spc_mfhi;
        case FUNCT_MTHI:    return mips_spc_mthi;
        case FUNCT_MFLO:    return mips_spc_mflo;
        case FUNCT_MTLO:    return mips_spc_mtlo;
        case FUNCT_MULT:    return mips_spc_mult;
        case FUNCT_MULTU:   return mips_spc_multu;
        case FUNCT_DIV:     return mips_spc_div;
        case FUNCT_DIVU:    return mips_spc_divu;
        case FUNCT_ADD:     return mips_spc_add;
        case FUNCT_ADDU:    return mips_spc_addu;
        case FUNCT_AND:     return mips_spc_and;
        case FUNCT_NOR:     return mips_spc_nor;
        case FUNCT_SUB:     return mips_spc_sub;
        case FUNCT_SUBU:    return mips_spc_subu;
        case FUNCT_OR:      return mips_spc_or;
        case FUNCT_XOR:     return mips_spc_xor;
        case FUNCT_SLT:     return mips_spc_slt;
        case FUNCT_SLTU:    return mips_spc_sltu;
        case FUNCT_TEQ:     return mips_spc_teq;
        case FUNCT_TNE:     return mips_spc_tne;
        case FUNCT_BREAK:   return mips_spc_break;
        case FUNCT_SYNC:    return mips_nop;
        default: {
            char buf[50];
            disassemble(pc, instr.raw, buf, 50);
            logfatal("other/unknown MIPS Special 0x%08X with FUNCT: %d%d%d%d%d%d [%s]", instr.raw,
                     instr.funct0, instr.funct1, instr.funct2, instr.funct3, instr.funct4, instr.funct5, buf);
        }
    }
}

INLINE mipsinstr_handler_t r3000a_regimm_decode(u32 pc, mips_instruction_t instr) {
    switch (instr.i.rt) {
        case RT_BLTZ:   return mips_ri_bltz;
        case RT_BGEZ:   return mips_ri_bgez;
        case RT_BLTZAL: return mips_ri_bltzal;
        case RT_BGEZAL: return mips_ri_bgezal;
        default: {
            char buf[50];
            disassemble(pc, instr.raw, buf, 50);
            logfatal("other/unknown MIPS REGIMM 0x%08X with RT: %d%d%d%d%d [%s]", instr.raw,
                     instr.rt0, instr.rt1, instr.rt2, instr.rt3, instr.rt4, buf);
        }
    }
}

mipsinstr_handler_t r3000a_instruction_decode(u32 pc, mips_instruction_t instr) {
#ifdef LOG_ENABLED
    char buf[50];
    if (ps1_log_verbosity >= LOG_VERBOSITY_DEBUG) {
        disassemble(pc, instr.raw, buf, 50);
        logdebug("[0x%08X]=0x%08X %s", pc, instr.raw, buf);
    }
#endif
    if (unlikely(instr.raw == 0)) {
        return mips_nop;
    }
    switch (instr.op) {
        case OPC_CP0:    return r3000a_cp0_decode(pc, instr);
        //case OPC_CP1:    return r3000a_cp1_decode(pc, instr);
        case OPC_SPCL:   return r3000a_special_decode(pc, instr);
        case OPC_REGIMM: return r3000a_regimm_decode(pc, instr);

        case OPC_LUI:    return mips_lui;
        case OPC_ADDIU:  return mips_addiu;
        case OPC_ADDI:   return mips_addi;
        case OPC_ANDI:   return mips_andi;
        case OPC_LBU:    return mips_lbu;
        case OPC_LHU:    return mips_lhu;
        case OPC_LH:     return mips_lh;
        case OPC_LW:     return mips_lw;
        case OPC_LWU:    return mips_lwu;
        case OPC_BEQ:    return mips_beq;
        case OPC_BGTZ:   return mips_bgtz;
        case OPC_BLEZ:   return mips_blez;
        case OPC_BNE:    return mips_bne;
        case OPC_CACHE:  return mips_cache;
        case OPC_SB:     return mips_sb;
        case OPC_SH:     return mips_sh;
        case OPC_SW:     return mips_sw;
        case OPC_ORI:    return mips_ori;
        case OPC_J:      return mips_j;
        case OPC_JAL:    return mips_jal;
        case OPC_SLTI:   return mips_slti;
        case OPC_SLTIU:  return mips_sltiu;
        case OPC_XORI:   return mips_xori;
        case OPC_LB:     return mips_lb;
        case OPC_LWL:    return mips_lwl;
        case OPC_LWR:    return mips_lwr;
        case OPC_SWL:    return mips_swl;
        case OPC_SWR:    return mips_swr;
        default:
#ifdef LOG_ENABLED
            if (ps1_log_verbosity < LOG_VERBOSITY_DEBUG) {
                disassemble(pc, instr.raw, buf, 50);
            }
            logfatal("Failed to decode instruction 0x%08X opcode %d%d%d%d%d%d [%s]",
                     instr.raw, instr.op0, instr.op1, instr.op2, instr.op3, instr.op4, instr.op5, buf);
#else
            logfatal("Failed to decode instruction 0x%08X opcode %d%d%d%d%d%d]",
                     instr.raw, instr.op0, instr.op1, instr.op2, instr.op3, instr.op4, instr.op5);
#endif
    }
}

INLINE u32 resolve_virtual_address(u32 pc) {
    return pc & 0x1FFFFFFF;
}

void cpu_step() {
    //PS1CPU.cp0.count += CYCLES_PER_INSTR;
    //PS1CPU.cp0.count &= 0x1FFFFFFFF;
    /*
    if (unlikely(PS1CPU.cp0.count == (dword)PS1CPU.cp0.compare << 1)) {
        PS1CPU.cp0.cause.ip7 = true;
        loginfo("Compare interrupt! count = 0x%09lX compare << 1 = 0x%09lX", PS1CP0.count, (dword)PS1CP0.compare << 1);
        r3000a_interrupt_update();
    }
     */

    /* Commented out for now since the game never actually reads cp0.random
    if (PS1CPU.cp0.random <= PS1CPU.cp0.wired) {
        PS1CPU.cp0.random = 31;
    } else {
        PS1CPU.cp0.random--;
    }
     */

    u32 pc = PS1CPU.pc;
    mips_instruction_t instruction;
    instruction.raw = ps1_read32(pc);

    if (unlikely(PS1CPU.interrupts > 0)) {
        if(PS1CPU.cp0.status.iec) {
            cpu_handle_exception(pc, EXCEPTION_INTERRUPT, -1);
            return;
        }
    }


    PS1CPU.prev_pc = PS1CPU.pc;
    PS1CPU.pc = PS1CPU.next_pc;
    PS1CPU.next_pc += 4;
    PS1CPU.branch = false;

    r3000a_instruction_decode(pc, instruction)(instruction);
    PS1CPU.exception = false; // only used in dynarec
}

void cpu_interrupt_update() {
    PS1CPU.interrupts = PS1CPU.cp0.cause.interrupt_pending & PS1CPU.cp0.status.im;
}