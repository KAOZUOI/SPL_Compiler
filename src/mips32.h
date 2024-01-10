#ifndef MIPS_H
#define MIPS_H

#include "tacReader.h"
#define FALSE 0
#define TRUE 1
typedef unsigned char bool;

typedef enum {
    zero, at, v0, v1, a0, a1, a2, a3,
    t0, t1, t2, t3, t4, t5, t6, t7,
    s0, s1, s2, s3, s4, s5, s6, s7,
    t8, t9, k0, k1, gp, sp, fp, ra, NUM_REGS
} Register;


struct RegDesc {    // the register descriptor
    const char *name;
    char var[8];
    bool dirty; // value updated but not stored
    /* add other fields as you need */
};
extern struct RegDesc regs[NUM_REGS];


struct VarDesc {    // the variable descriptor
    char var[8];
    Register reg;
    int offset; // the offset from stack
    /* add other fields as you need */
    struct VarDesc *next;
};
extern struct VarDesc *vars;

typedef struct VarNode {
    char var_name[100];  // 假设变量名不超过100个字符
    struct VarNode *next;
} VarNode;

void mips32_gen(tac *head, FILE *_fd);

Register spill_register(tac_opd *opd);

#endif // MIPS_H
