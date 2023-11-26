#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "hash/uthash.h"
#include "type.h"


typedef struct Symbol{
    char* name;
    Type* type;
    UT_hash_handle hh; // makes this structure hashable
} Symbol;

extern Symbol* symbolTable;

void insertSymbol(Symbol* s); // insert symbol into hash table
Symbol* findSymbol(const char* name); // find symbol according to name
void deleteAll(); // delete all symbols in hash table

#endif