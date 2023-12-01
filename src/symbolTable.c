#include "symbolTable.h"
#include <stdio.h>
Symbol* symbolTable = NULL;

void insertSymbol(Symbol* s) {
    HASH_ADD_STR(symbolTable, name, s);
}

Symbol* findSymbol(const char* name) {
    Symbol* s;
    #ifdef DEBUG
    printf("findSymbol_in\n");
    #endif
    HASH_FIND_STR(symbolTable, name, s);
    #ifdef DEBUG
    printf("findSymbol_out\n");
    #endif
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

void printSymbolTable() {
    Symbol* currentSymbol;
    Symbol* tmp;
    HASH_ITER(hh, symbolTable, currentSymbol, tmp) {
        printf("name: %s\n", currentSymbol->name);
        printf("type: %s\n", currentSymbol->type->name);
    }
}

void deleteSymbol(Symbol* s) {
    HASH_DEL(symbolTable, s);
    free(s->name);
    free(s);
}

