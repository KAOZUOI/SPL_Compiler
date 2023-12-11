#ifndef _TAC
#define _TAC
#include <stdio.h>
typedef struct tac
{
    enum { FUNC = 0, READ, WRITE, LABEL, GOTO, IF, ASS, OPER, RETURN, PARAM, ARG, DEC} title;
    char* target;
    char* op;
    char* arg1;
    char* arg2;
    struct tac* before;
    struct tac* next;
} Tac;

Tac * insertTac(char* target, char* op, char* arg1, char* arg2);
void printTacList(FILE *file);
void freeTacList();

#endif
