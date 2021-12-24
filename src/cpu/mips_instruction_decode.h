#ifndef N64_MIPS_INSTRUCTION_DECODE_H
#define N64_MIPS_INSTRUCTION_DECODE_H

#include <util.h>

typedef union mips_instruction {
    u32 raw;

    struct {
        unsigned:26;
        unsigned op5:1;
        unsigned op4:1;
        unsigned op3:1;
        unsigned op2:1;
        unsigned op1:1;
        unsigned op0:1;
    } PACKED;

    struct {
        unsigned:26;
        unsigned op:6;
    } PACKED;

    struct {
        unsigned immediate:16;
        unsigned rt:5;
        unsigned rs:5;
        unsigned op:6;
    } PACKED i;

    struct {
        unsigned offset:16;
        unsigned ft:5;
        unsigned base:5;
        unsigned op:6;
    } PACKED fi;

    struct {
        unsigned target:26;
        unsigned op:6;
    } PACKED j;

    struct {
        unsigned funct:6;
        unsigned sa:5;
        unsigned rd:5;
        unsigned rt:5;
        unsigned rs:5;
        unsigned op:6;
    } PACKED r;

    struct {
        unsigned funct:6;
        unsigned fd:5;
        unsigned fs:5;
        unsigned ft:5;
        unsigned fmt:5;
        unsigned op:6;
    } PACKED fr;

    struct {
        unsigned offset:7;
        unsigned element:4;
        unsigned funct:5;
        unsigned vt:5;
        unsigned base:5;
        unsigned op:6;
    } PACKED v;

    struct {
        unsigned funct:6;
        unsigned vd:5;
        unsigned vs:5;
        unsigned vt:5;
        unsigned e:4;
        unsigned is_vec:1;
        unsigned op:6;
    } PACKED cp2_vec;

    struct {
        unsigned:7;
        unsigned e:4;
        unsigned rd:5;
        unsigned rt:5;
        unsigned funct:5;
        unsigned op:6;
    } PACKED cp2_regmove;

    struct {
        unsigned funct5:1;
        unsigned funct4:1;
        unsigned funct3:1;
        unsigned funct2:1;
        unsigned funct1:1;
        unsigned funct0:1;
        unsigned:26;
    } PACKED;

    struct {
        unsigned:16;
        unsigned rt4:1;
        unsigned rt3:1;
        unsigned rt2:1;
        unsigned rt1:1;
        unsigned rt0:1;
        unsigned:11;
    } PACKED;

    struct {
        unsigned:21;
        unsigned rs4:1;
        unsigned rs3:1;
        unsigned rs2:1;
        unsigned rs1:1;
        unsigned rs0:1;
        unsigned:6;
    } PACKED;

    struct {
        unsigned last11:11;
        unsigned:21;
    } PACKED;

} PACKED mips_instruction_t;

ASSERT32(mips_instruction_t);

#endif //N64_MIPS_INSTRUCTION_DECODE_H
