#ifndef PS1_CPU_H
#define PS1_CPU_H
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <util.h>
#include <log.h>
#include "mips_instruction_decode.h"

// Exceptions
#define EXCEPTION_INTERRUPT            0
#define EXCEPTION_ADDRESS_ERROR_LOAD   4
#define EXCEPTION_ADDRESS_ERROR_STORE  5
#define EXCEPTION_BUS_ERROR_INS_FETCH  6
#define EXCEPTION_BUS_ERROR_LOAD_STORE 7
#define EXCEPTION_SYSCALL              8
#define EXCEPTION_BREAKPOINT           9
#define EXCEPTION_RESERVED_INSTR       10
#define EXCEPTION_COPROCESSOR_UNUSABLE 11
#define EXCEPTION_ARITHMETIC_OVERFLOW  12
#define EXCEPTION_TRAP                 13
#define EXCEPTION_FLOATING_POINT       15
#define EXCEPTION_WATCH                23

#define CPU_REG_LR 31

#define CP0_REG_0        0
#define CP0_REG_1        1
#define CP0_REG_2        2
#define CP0_REG_BPC      3
#define CP0_REG_4        4
#define CP0_REG_BDA      5
#define CP0_REG_JUMPDEST 6
#define CP0_REG_DCIC     7
#define CP0_REG_BADVADDR 8
#define CP0_REG_BDAM     9
#define CP0_REG_10       10
#define CP0_REG_BPCM     11
#define CP0_REG_SR       12
#define CP0_REG_CAUSE    13
#define CP0_REG_EPC      14
#define CP0_REG_PRID     15
#define CP0_REG_16       16
#define CP0_REG_17       17
#define CP0_REG_18       18
#define CP0_REG_19       19
#define CP0_REG_20       20
#define CP0_REG_21       21
#define CP0_REG_22       22
#define CP0_REG_23       23
#define CP0_REG_24       24
#define CP0_REG_25       25
#define CP0_REG_26       26
#define CP0_REG_27       27
#define CP0_REG_28       28
#define CP0_REG_29       29
#define CP0_REG_30       30
#define CP0_REG_31       31

#define CP0_CONFIG_WRITE_MASK 0x0FFFFFFF

#define CPU_MODE_KERNEL 0
#define CPU_MODE_SUPERVISOR 1 /* TODO this is probably wrong */
#define CPU_MODE_USER 2 /* TODO this is probably wrong */

#define OPC_CP0    0b010000
#define OPC_CP1    0b010001
#define OPC_CP2    0b010010
#define OPC_LD     0b110111
#define OPC_LUI    0b001111
#define OPC_ADDI   0b001000
#define OPC_ADDIU  0b001001
#define OPC_ANDI   0b001100
#define OPC_LBU    0b100100
#define OPC_LHU    0b100101
#define OPC_LH     0b100001
#define OPC_LW     0b100011
#define OPC_LWU    0b100111
#define OPC_BEQ    0b000100
#define OPC_BGTZ   0b000111
#define OPC_BLEZ   0b000110
#define OPC_BNE    0b000101
#define OPC_CACHE  0b101111
#define OPC_REGIMM 0b000001
#define OPC_SPCL   0b000000
#define OPC_SB     0b101000
#define OPC_SH     0b101001
#define OPC_SW     0b101011
#define OPC_ORI    0b001101
#define OPC_J      0b000010
#define OPC_JAL    0b000011
#define OPC_SLTI   0b001010
#define OPC_SLTIU  0b001011
#define OPC_XORI   0b001110
#define OPC_LB     0b100000
#define OPC_LWL    0b100010
#define OPC_LWR    0b100110
#define OPC_SWL    0b101010
#define OPC_SWR    0b101110

// Coprocessor
#define COP_MF    0b00000
#define COP_MT    0b00100


// Coprocessor FUNCT
#define COP_FUNCT_WAIT       0b100000
#define COP_FUNCT_RFE        0b010000

// Special
#define FUNCT_SLL     0b000000
#define FUNCT_SRL     0b000010
#define FUNCT_SRA     0b000011
#define FUNCT_SRAV    0b000111
#define FUNCT_SLLV    0b000100
#define FUNCT_SRLV    0b000110
#define FUNCT_JR      0b001000
#define FUNCT_JALR    0b001001
#define FUNCT_SYSCALL 0b001100
#define FUNCT_SYNC    0b001111
#define FUNCT_MFHI    0b010000
#define FUNCT_MTHI    0b010001
#define FUNCT_MFLO    0b010010
#define FUNCT_MTLO    0b010011
#define FUNCT_MULT    0b011000
#define FUNCT_MULTU   0b011001
#define FUNCT_DIV     0b011010
#define FUNCT_DIVU    0b011011
#define FUNCT_ADD     0b100000
#define FUNCT_ADDU    0b100001
#define FUNCT_AND     0b100100
#define FUNCT_SUB     0b100010
#define FUNCT_SUBU    0b100011
#define FUNCT_OR      0b100101
#define FUNCT_XOR     0b100110
#define FUNCT_NOR     0b100111
#define FUNCT_SLT     0b101010
#define FUNCT_SLTU    0b101011
#define FUNCT_TEQ     0b110100
#define FUNCT_TNE     0b110110

#define FUNCT_BREAK 0b001101


// REGIMM
#define RT_BLTZ   0b00000
#define RT_BGEZ   0b00001
#define RT_BLTZAL 0b10000
#define RT_BGEZAL 0b10001


typedef union cp0_status {
    u32 raw;
    struct {
        unsigned ie_ku:6;
        unsigned:26;
    };
    struct {
        unsigned iec:1; // Current interrupt enable
        unsigned kuc:1; // Current kernel/user mode switch (0 = kernel mode, 1 = user mode)
        unsigned iep:1; // Previous interrupt enable
        unsigned kup:1; // Previous kernel/user mode switch
        unsigned ieo:1; // Old interrupt enable
        unsigned kuo:1; // Old kernel/user mode switch
        unsigned:2;
        unsigned im:8;  // Interrupt mask
        unsigned isc:1; // Isolate cache
        unsigned swc:1; // Swap cache
        unsigned pz:1;  // Write cache parity as 0?
        unsigned cm:1;
        unsigned pe:1;
        unsigned ts:1;
        unsigned bev:1;
        unsigned:2;
        unsigned re:1;
        unsigned:2;
        unsigned cu0:1;
        unsigned cu1:1;
        unsigned cu2:1;
        unsigned cu3:1;
    } PACKED;
} PACKED cp0_status_t;

ASSERT32(cp0_status_t);

typedef union cp0_cause {
    struct {
        unsigned:8;
        unsigned interrupt_pending:8;
        unsigned:16;
    };
    struct {
        unsigned:2;
        unsigned exception_code:5;
        unsigned:1;
        unsigned ip0:1;
        unsigned ip1:1;
        unsigned ip2:1;
        unsigned ip3:1;
        unsigned ip4:1;
        unsigned ip5:1;
        unsigned ip6:1;
        unsigned ip7:1;
        unsigned:12;
        unsigned coprocessor_error:2;
        unsigned:1;
        unsigned branch_delay:1;
    };
    u32 raw;
} cp0_cause_t;

ASSERT32(cp0_cause_t);

typedef union cp0_entry_lo {
    u32 raw;
    struct {
        unsigned g:1;
        unsigned v:1;
        unsigned d:1;
        unsigned c:3;
        unsigned pfn:20;
        unsigned:6;
    };
} cp0_entry_lo_t;

ASSERT32(cp0_entry_lo_t);

typedef union cp0_page_mask {
    u32 raw;
    struct {
        unsigned:13;
        unsigned mask:12;
        unsigned:7;
    };
} cp0_page_mask_t;

ASSERT32(cp0_page_mask_t);

typedef union cp0_entry_hi {
    struct {
        unsigned asid:8;
        unsigned:5;
        unsigned vpn2:19;
    };
    u32 raw;
} cp0_entry_hi_t;

ASSERT32(cp0_entry_hi_t);


typedef union watch_lo {
    u32 raw;
    struct {
        unsigned w:1;
        unsigned r:1;
        unsigned:1;
        unsigned paddr0:29;
    };
} watch_lo_t;

ASSERT32(watch_lo_t);

typedef union cp0_context {
    u32 raw;
    struct {
        unsigned:4;
        unsigned badvpn2:19;
        unsigned ptebase:9;
    };
} cp0_context_t;

ASSERT32(cp0_context_t);

typedef struct cp0 {
    /*
    word index;
    word random;
    cp0_entry_lo_t entry_lo0;
    cp0_entry_lo_t entry_lo1;
    cp0_context_t context;
    dword context_64;
    cp0_page_mask_t page_mask;
    word wired;
    word r7;
    dword bad_vaddr;
     */
    u64 count;
    /*
    cp0_entry_hi_t entry_hi;
    cp0_entry_hi_64_t entry_hi_64;
    word compare;
     */
    cp0_status_t status;
    cp0_cause_t cause;
    u32 EPC;
    /*
    word PRId;
    word config;
    word lladdr;
    watch_lo_t watch_lo;
    word watch_hi;
    cp0_x_context_t x_context;
    word r21;
    word r22;
    word r23;
    word r24;
    word r25;
    word parity_error;
    word cache_error;
    word tag_lo;
    word tag_hi;
     */
    u32 error_epc;
    /*
    word r31;

    tlb_entry_t    tlb[32];

     */
    bool kernel_mode;
    bool user_mode;
    bool isolate_cache;
} cp0_t;


typedef struct r3000a {
    u32 gpr[32];

    u32 pc;
    u32 next_pc;
    u32 prev_pc;

    u32 mult_hi;
    u32 mult_lo;

    bool llbit;

    cp0_t cp0;

    // Cached value of `cp0.cause.interrupt_pending & cp0.status.im`
    u8 interrupts;

    // In a branch delay slot?
    bool branch;

    // Did an exception just happen?
    bool exception;
} r3000a_t;

extern r3000a_t ps1cpu;
#define PS1CPU ps1cpu
#define PS1CP0 PS1CPU.cp0

typedef void(*mipsinstr_handler_t)(mips_instruction_t);

void cpu_step();
void cpu_handle_exception(u32 pc, u32 code, s32 coprocessor_error);
mipsinstr_handler_t cpu_instruction_decode(u32 pc, mips_instruction_t instr);
void cpu_interrupt_update();
bool instruction_stable(mips_instruction_t instr);

extern const char* register_names[];
extern const char* cp0_register_names[];

INLINE void cpu_set_pc(u32 new_pc) {
    PS1CPU.prev_pc = PS1CPU.pc;
    PS1CPU.pc = new_pc;
    PS1CPU.next_pc = PS1CPU.pc + 4;
}

INLINE void cp0_status_updated() {
    PS1CP0.user_mode     = PS1CP0.status.kuc == 1;
    PS1CP0.kernel_mode   = PS1CP0.status.kuc == 0;
    PS1CP0.isolate_cache = PS1CP0.status.isc == 1;

}

#endif //PS1_CPU_H
