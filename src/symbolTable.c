#include "symbolTable.h"

Symbol* symbolTable = NULL;

void insertSymbol(Symbol* s) {
    HASH_ADD_STR(symbolTable, name, s);
}

Symbol* findSymbol(const char* name) {
    Symbol* s;
    HASH_FIND_STR(symbolTable, name, s);
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

