#include "mips32.h"

/* the output file descriptor, may not be explicitly used */
FILE *fd;

#define _tac_kind(tac) (((tac)->code).kind)
#define _tac_quadruple(tac) (((tac)->code).tac)
#define _reg_name(reg) regs[reg].name


struct RegDesc regs[NUM_REGS];
struct VarDesc *vars = NULL;
VarNode *all_variables = NULL;
int all_variables_count = 0;
int arg_count = 0;
int param_count = 0;

void add_variable(const char *var_name) {
    // 首先检查变量是否已存在于列表中
    VarNode *current = all_variables;
    // _mips_printf("have");
    while (current != NULL) {
        if (strcmp(current->var_name, var_name) == 0) {
            return; // 变量已存在，无需添加
        }
        current = current->next;
    }
    // _mips_printf("add");
    // 创建新节点并添加到列表中
    VarNode *new_node = (VarNode *)malloc(sizeof(VarNode));
    strcpy(new_node->var_name, var_name);
    new_node->next = all_variables;
    all_variables = new_node;
}

// 添加新的局部变量
void add_local_var(const char *var_name, int offset) {
    struct VarDesc *new_var = (struct VarDesc*)malloc(sizeof(struct VarDesc));
    strcpy(new_var->var, var_name);
    new_var->reg = -1;  // 表示不在寄存器中
    new_var->offset = offset;
    new_var->next = vars;
    vars = new_var;
}

void _mips_printf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);
    fputs("\n", fd);
}

void _mips_iprintf(const char *fmt, ...){
    va_list args;
    fputs("  ", fd); // `iprintf` stands for indented printf
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);
    fputs("\n", fd);
}

// 函数开始的offset初始值
static int local_var_offset;



struct VarDesc *find_var_desc(const char *var_name) {
    struct VarDesc *current = vars;
    while (current != NULL) {
        if (strcmp(current->var, var_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void store_register_to_memory(Register reg, const char *var_name) {
    struct VarDesc *var_desc = find_var_desc(var_name);
    if (var_desc != NULL) {
        _mips_iprintf("sw %s, %d($fp)", _reg_name(reg), var_desc->offset);
    }
}


Register get_register(tac_opd *opd) {
    // 检查该变量是否已经分配了寄存器
    for (int i = t0; i <= t9; i++) {
        if (strcmp(regs[i].var, opd->char_val) == 0) {
            return i; // 返回已分配的寄存器
        }
    }

    // 分配一个新的临时寄存器
    for (int i = t0; i <= t9; i++) {
        if (strlen(regs[i].var) == 0) {
            strcpy(regs[i].var, opd->char_val);
            return i; // 返回新分配的寄存器
        }
    }

    // 所有临时寄存器都在使用中，需要溢出
    return spill_register(opd);
}



Register get_register_w(tac_opd *opd) {
    // 与 get_register 相同的逻辑，但标记分配的寄存器为 dirty
    Register reg = get_register(opd);
    regs[reg].dirty = TRUE;
    return reg;
}


Register spill_register(tac_opd *opd) {
    for (int i = t0; i <= t9; i++) {
        if (regs[i].dirty) {
            store_register_to_memory(i, regs[i].var);
            regs[i].dirty = FALSE;
            strcpy(regs[i].var, opd->char_val);
            return i;
        }
    }

    store_register_to_memory(t0, regs[t0].var);
    regs[t0].dirty = FALSE;
    strcpy(regs[t0].var, opd->char_val);
    return t0;
}







char* loadVar2Reg(tac_opd *opd, int registerNumber) {
    if (opd == NULL) {
        _mips_printf("Error: NULL operand in loadVar2Reg\n");
        return NULL;
    }

    char* buffer = (char*)malloc(100 * sizeof(char)); // 分配足够大的缓冲区
    if (!buffer) {
        _mips_printf("Error: Memory allocation failed in loadVar2Reg\n");
        return NULL;
    }

    if (opd->kind == OP_CONSTANT) {
        if (opd->int_val == 0) {
            sprintf(buffer, "move $t%d, $zero", registerNumber);
        } else {
            sprintf(buffer, "li $t%d, %d", registerNumber, opd->int_val);
        }
    } else if (opd->kind == OP_VARIABLE) {
        // 从标签地址加载变量到寄存器
        sprintf(buffer, "lw $t%d, _%s", registerNumber, opd->char_val);
    } else {
        _mips_printf("Error: Unsupported operand type in loadVar2Reg\n");
        free(buffer);
        return NULL;
    }

    return buffer;
}


void emit_data_section(FILE *fd) {
    fprintf(fd, ".data\n");
    VarNode *current = all_variables;
    while (current != NULL) {
        fprintf(fd, "_%s: .word 0\n", current->var_name);
        current = current->next;
    }
}

/* PARAM: a pointer to `struct tac_node` instance
   RETURN: the next instruction to be translated */
tac *emit_label(tac *label){
    assert(_tac_kind(label) == LABEL);
    _mips_printf("label%d:", _tac_quadruple(label).labelno->int_val);
    return label->next;
}

tac *emit_function(tac *function) {
    local_var_offset = -4; // 初始化局部变量的offset
    _mips_printf("%s:", _tac_quadruple(function).funcname);
    return function->next;
}


tac *emit_assign(tac *assign) {
    // 添加变量到列表（如果它们尚未存在）
    add_variable(_tac_quadruple(assign).left->char_val);
    if (_tac_quadruple(assign).right->kind != OP_CONSTANT) {
        add_variable(_tac_quadruple(assign).right->char_val);
    }

    // 右侧值的加载
    char *mipsLwBufRight = loadVar2Reg(_tac_quadruple(assign).right, 0);

    // 执行赋值操作
    _mips_iprintf("%s", mipsLwBufRight);
    _mips_iprintf("sw $t0, _%s", _tac_quadruple(assign).left->char_val);

    // 释放动态分配的内存
    free(mipsLwBufRight);

    return assign->next;
}

tac *emit_add(tac *add){
    // 加载右操作数
    char *mipsLwBuf1 = loadVar2Reg(_tac_quadruple(add).r1, 0);
    char *mipsLwBuf2 = loadVar2Reg(_tac_quadruple(add).r2, 1);
    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);

    // 执行加法
    _mips_iprintf("add $t2, $t0, $t1");

    // 结果存回内存
    _mips_iprintf("sw $t2, _%s", _tac_quadruple(add).left->char_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return add->next;
}


tac *emit_sub(tac *sub) {
    char *mipsLwBuf1 = loadVar2Reg(_tac_quadruple(sub).r1, 0);
    char *mipsLwBuf2 = loadVar2Reg(_tac_quadruple(sub).r2, 1);
    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);

    _mips_iprintf("sub $t2, $t0, $t1");
    _mips_iprintf("sw $t2, _%s", _tac_quadruple(sub).left->char_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return sub->next;
}

tac *emit_mul(tac *mul) {
    char *mipsLwBuf1 = loadVar2Reg(_tac_quadruple(mul).r1, 0);
    char *mipsLwBuf2 = loadVar2Reg(_tac_quadruple(mul).r2, 1);
    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);

    _mips_iprintf("mul $t2, $t0, $t1");
    _mips_iprintf("sw $t2, _%s", _tac_quadruple(mul).left->char_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return mul->next;
}

tac *emit_div(tac *div) {
    char *mipsLwBuf1 = loadVar2Reg(_tac_quadruple(div).r1, 0);
    char *mipsLwBuf2 = loadVar2Reg(_tac_quadruple(div).r2, 1);
    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);

    _mips_iprintf("div $t0, $t1");
    _mips_iprintf("mflo $t2");
    _mips_iprintf("sw $t2, _%s", _tac_quadruple(div).left->char_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return div->next;
}

tac *emit_addr(tac *addr){
    Register x, y;

    x = get_register_w(_tac_quadruple(addr).left);
    y = get_register(_tac_quadruple(addr).right);
    _mips_iprintf("move %s, %s", _reg_name(x), _reg_name(y));
    return addr->next;
}

tac *emit_fetch(tac *fetch){
    Register x, y;

    x = get_register_w(_tac_quadruple(fetch).left);
    y = get_register(_tac_quadruple(fetch).raddr);
    _mips_iprintf("lw %s, 0(%s)", _reg_name(x), _reg_name(y));
    return fetch->next;
}

tac *emit_deref(tac *deref){
    Register x, y;

    x = get_register(_tac_quadruple(deref).laddr);
    y = get_register(_tac_quadruple(deref).right);
    _mips_iprintf("sw %s, 0(%s)", _reg_name(y), _reg_name(x));
    return deref->next;
}

tac *emit_goto(tac *goto_){
    _mips_iprintf("j label%d", _tac_quadruple(goto_).labelno->int_val);
    return goto_->next;
}

tac *emit_iflt(tac *iflt){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(iflt).c1, 0); // 使用 $t0
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(iflt).c2, 1); // 使用 $t1

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("blt $t0, $t1, label%d", _tac_quadruple(iflt).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return iflt->next;
}

tac *emit_ifle(tac *ifle){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(ifle).c1, 0); // 使用 $t0
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(ifle).c2, 1); // 使用 $t1

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("ble $t0, $t1, label%d", _tac_quadruple(ifle).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return ifle->next;
}

tac *emit_ifgt(tac *ifgt){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(ifgt).c1, 0);
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(ifgt).c2, 1);

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("bgt $t0, $t1, label%d", _tac_quadruple(ifgt).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return ifgt->next;
}

tac *emit_ifge(tac *ifge){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(ifge).c1, 0);
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(ifge).c2, 1);

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("bge $t0, $t1, label%d", _tac_quadruple(ifge).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return ifge->next;
}

tac *emit_ifne(tac *ifne){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(ifne).c1, 0);
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(ifne).c2, 1);

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("bne $t0, $t1, label%d", _tac_quadruple(ifne).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return ifne->next;
}

tac *emit_ifeq(tac *ifeq){
    char* mipsLwBuf1 = loadVar2Reg(_tac_quadruple(ifeq).c1, 0);
    char* mipsLwBuf2 = loadVar2Reg(_tac_quadruple(ifeq).c2, 1);

    _mips_iprintf("%s", mipsLwBuf1);
    _mips_iprintf("%s", mipsLwBuf2);
    _mips_iprintf("beq $t0, $t1, label%d", _tac_quadruple(ifeq).labelno->int_val);

    free(mipsLwBuf1);
    free(mipsLwBuf2);

    return ifeq->next;
}

tac *emit_return(tac *return_) {
    _mips_iprintf("lw $v0, _%s", _tac_quadruple(return_).var->char_val);
    _mips_iprintf("jr $ra");
    return return_->next;
}

tac *emit_dec(tac *dec) {
    add_local_var(_tac_quadruple(dec).var->char_val, local_var_offset);
    local_var_offset -= 4; // 假设每个变量占用4字节
    return dec->next;
}




tac *emit_arg(tac *arg) {
    char* mipsLwBuf = loadVar2Reg(_tac_quadruple(arg).var, 0);
    _mips_iprintf("%s", mipsLwBuf);
    // 将参数推送到栈上
    _mips_iprintf("addi $sp, $sp, -4");
    _mips_iprintf("sw $t0, 0($sp)");
    arg_count++;
    return arg->next;
}




tac *emit_call(tac *call) {
    // 将所有变量保存到栈上
    _mips_iprintf("addi $sp, $sp, -%d", all_variables_count * 4 + 4);
    int i = 0;
    VarNode *all_variables_copy = all_variables;
    while (all_variables_copy != NULL) {
        _mips_iprintf("lw $t0, _%s", all_variables_copy->var_name);
        _mips_iprintf("sw $t0, %d($sp)", i * 4);
        i++;
        all_variables_copy = all_variables_copy->next;
    }
    _mips_iprintf("sw $ra, %d($sp)", i * 4);

    param_count = 0;
    _mips_iprintf("jal %s", _tac_quadruple(call).funcname); // 跳转并链接

    // 恢复所有变量
    all_variables_copy = all_variables;
    i = 0;
    while (all_variables_copy != NULL) {
        _mips_iprintf("lw $t0, %d($sp)", i * 4);
        _mips_iprintf("sw $t0, _%s", all_variables_copy->var_name);
        i++;
        all_variables_copy = all_variables_copy->next;
    }
    _mips_iprintf("lw $ra, %d($sp)", i * 4);
    _mips_iprintf("addi $sp, $sp, %d", all_variables_count * 4 + 4 + arg_count * 4);
    // 处理函数返回值
    if (_tac_quadruple(call).ret != NULL) {
        _mips_iprintf("sw $v0, _%s", _tac_quadruple(call).ret->char_val);
    }

    // 重置参数计数
    arg_count = 0;

    return call->next;
}





tac *emit_param(tac *param) {
    _mips_iprintf("lw $t0, %d($sp)", param_count * 4 + all_variables_count * 4 + 4);
    _mips_iprintf("sw $t0, _%s", _tac_quadruple(param).p->char_val);

    param_count++;

    return param->next;
}



tac *emit_read(tac *read){
    _mips_iprintf("addi $sp, $sp, -4");
    _mips_iprintf("sw $ra, 0($sp)");
    _mips_iprintf("jal read");
    _mips_iprintf("lw $ra, 0($sp)");
    _mips_iprintf("addi $sp, $sp, 4");
    _mips_iprintf("sw $v0, _%s", _tac_quadruple(read).p->char_val);
    return read->next;
}

tac *emit_write(tac *write){
    _mips_iprintf("lw $a0, _%s", _tac_quadruple(write).p->char_val);
    _mips_iprintf("addi $sp, $sp, -4");
    _mips_iprintf("sw $ra, 0($sp)");
    _mips_iprintf("jal write");
    _mips_iprintf("lw $ra, 0($sp)");
    _mips_iprintf("addi $sp, $sp, 4");
    return write->next;
}

void emit_preamble(){
    
    _mips_printf("_prmpt: .asciiz \"Enter an integer: \"");
    _mips_printf("_eol: .asciiz \"\\n\"");
    _mips_printf(".globl main");
    _mips_printf(".text");
}

void emit_read_function(){
    _mips_printf("read:");
    _mips_iprintf("li $v0, 4");
    _mips_iprintf("la $a0, _prmpt");
    _mips_iprintf("syscall");
    _mips_iprintf("li $v0, 5");
    _mips_iprintf("syscall");
    _mips_iprintf("jr $ra");
}

void emit_write_function(){
    _mips_printf("write:");
    _mips_iprintf("li $v0, 1");
    _mips_iprintf("syscall");
    _mips_iprintf("li $v0, 4");
    _mips_iprintf("la $a0, _eol");
    _mips_iprintf("syscall");
    _mips_iprintf("move $v0, $0");
    _mips_iprintf("jr $ra");
}

static tac* (*emitter[])(tac*) = {
    emit_label, emit_function, emit_assign,
    emit_add, emit_sub, emit_mul, emit_div,
    emit_addr, emit_fetch, emit_deref, emit_goto,
    emit_iflt, emit_ifle, emit_ifgt, emit_ifge, emit_ifne, emit_ifeq,
    emit_return, emit_dec, emit_arg, emit_call, emit_param,
    emit_read, emit_write
};

void collect_variables(tac *head) {
    for (tac *current_tac = head; current_tac != NULL; current_tac = current_tac->next) {
        if (current_tac->code.kind == ASSIGN) {
            if (current_tac->code.assign.left->kind == OP_VARIABLE) {
                add_variable(current_tac->code.assign.left->char_val);
            }
            if (current_tac->code.assign.right->kind == OP_VARIABLE) {
                add_variable(current_tac->code.assign.right->char_val);
            }
        }
        if (current_tac->code.kind == ADD) {
            if (current_tac->code.add.left->kind == OP_VARIABLE) {
                add_variable(current_tac->code.add.left->char_val);
            }
            if (current_tac->code.add.r1->kind == OP_VARIABLE) {
                add_variable(current_tac->code.add.r1->char_val);
            }
            if (current_tac->code.add.r2->kind == OP_VARIABLE) {
                add_variable(current_tac->code.add.r2->char_val);
            }
        }
        if (current_tac->code.kind == SUB) {
            if (current_tac->code.sub.left->kind == OP_VARIABLE) {
                add_variable(current_tac->code.sub.left->char_val);
            }
            if (current_tac->code.sub.r1->kind == OP_VARIABLE) {
                add_variable(current_tac->code.sub.r1->char_val);
            }
            if (current_tac->code.sub.r2->kind == OP_VARIABLE) {
                add_variable(current_tac->code.sub.r2->char_val);
            }
        }
        if (current_tac->code.kind == MUL) {
            if (current_tac->code.mul.left->kind == OP_VARIABLE) {
                add_variable(current_tac->code.mul.left->char_val);
            }
            if (current_tac->code.mul.r1->kind == OP_VARIABLE) {
                add_variable(current_tac->code.mul.r1->char_val);
            }
            if (current_tac->code.mul.r2->kind == OP_VARIABLE) {
                add_variable(current_tac->code.mul.r2->char_val);
            }
        }
        if (current_tac->code.kind == DIV) {
            if (current_tac->code.div.left->kind == OP_VARIABLE) {
                add_variable(current_tac->code.div.left->char_val);
            }
            if (current_tac->code.div.r1->kind == OP_VARIABLE) {
                add_variable(current_tac->code.div.r1->char_val);
            }
            if (current_tac->code.div.r2->kind == OP_VARIABLE) {
                add_variable(current_tac->code.div.r2->char_val);
            }
        }
        if(current_tac->code.kind == READ){
            if(current_tac->code.read.p->kind == OP_VARIABLE){
                add_variable(current_tac->code.read.p->char_val);
            }
        }
        if(current_tac->code.kind == WRITE){
            if(current_tac->code.write.p->kind == OP_VARIABLE){
                add_variable(current_tac->code.write.p->char_val);
            }
        }
        if(current_tac->code.kind == CALL){
            if(current_tac->code.call.ret->kind == OP_VARIABLE){
                add_variable(current_tac->code.call.ret->char_val);
            }
        }
    }
}

void count_variables(tac *head) {
    for (VarNode *current = all_variables; current != NULL; current = current->next) {
        all_variables_count++;
    }
}

tac *emit_code(tac *head){
    tac *(*tac_emitter)(tac*);
    tac *tac_code = head;
    collect_variables(head);
    count_variables(head);
    emit_data_section(fd);
    emit_preamble();
    emit_read_function();
    emit_write_function();
    
    while(tac_code != NULL){
        if(_tac_kind(tac_code) != NONE){
            tac_emitter = emitter[_tac_kind(tac_code)];
            tac_code = tac_emitter(tac_code);
        }
        else{
            tac_code = tac_code->next;
        }
    }
    
}

/* translate a TAC list into mips32 assembly
   output the textual assembly code to _fd */
void mips32_gen(tac *head, FILE *_fd){
    regs[zero].name = "$zero";
    regs[at].name = "$at";
    regs[v0].name = "$v0"; regs[v1].name = "$v1";
    regs[a0].name = "$a0"; regs[a1].name = "$a1";
    regs[a2].name = "$a2"; regs[a3].name = "$a3";
    regs[t0].name = "$t0"; regs[t1].name = "$t1";
    regs[t2].name = "$t2"; regs[t3].name = "$t3";
    regs[t4].name = "$t4"; regs[t5].name = "$t5";
    regs[t6].name = "$t6"; regs[t7].name = "$t7";
    regs[s0].name = "$s0"; regs[s1].name = "$s1";
    regs[s2].name = "$s2"; regs[s3].name = "$s3";
    regs[s4].name = "$s4"; regs[s5].name = "$s5";
    regs[s6].name = "$s6"; regs[s7].name = "$s7";
    regs[t8].name = "$t8"; regs[t9].name = "$t9";
    regs[k0].name = "$k0"; regs[k1].name = "$k1";
    regs[gp].name = "$gp";
    regs[sp].name = "$sp"; regs[fp].name = "$fp";
    regs[ra].name = "$ra";
    vars = (struct VarDesc*)malloc(sizeof(struct VarDesc));
    vars->next = NULL;
    // 初始化寄存器描述符
    for (int i = 0; i < NUM_REGS; i++) {
        regs[i].var[0] = '\0';
        regs[i].dirty = FALSE;
    }

    fd = _fd;
    emit_code(head);
}
