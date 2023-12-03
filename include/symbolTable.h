#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "hash/uthash.h"
#include "type.h"


typedef struct Symbol{
    char* name;
    Type* type;
    UT_hash_handle hh; // makes this structure hashable
} Symbol;

typedef struct ScopeNode {
    Symbol* symbolTable;
    struct ScopeNode* next;
} ScopeNode;

extern ScopeNode* scopeStack;

void pushScope();
void popScope();
void insertSymbol(Symbol* s);
Symbol* findSymbol(const char* name);
void deleteAll(Symbol* symbolTable);

#endif