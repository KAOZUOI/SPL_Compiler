#include "symbolTable.h"
#include <stdio.h>
Symbol* symbolTable = NULL;

void insertSymbol(Symbol* s) {
    HASH_ADD_STR(symbolTable, name, s);
}

Symbol* findSymbol(const char* name) {
    Symbol* s;
    printf("findSymbol_in\n");
    HASH_FIND_STR(symbolTable, name, s);
    printf("findSymbol_out\n");
    return s;
}

void deleteAll() {
    Symbol* currentSymbol;
    Symbol* tmp;
    HASH_ITER(hh, symbolTable, currentSymbol, tmp) {
        HASH_DEL(symbolTable, currentSymbol);
        free(currentSymbol->name);
        free(currentSymbol);
    }
}

