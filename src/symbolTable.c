#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>

ScopeNode* scopeStack = NULL;

void pushScope() {
    ScopeNode* newNode = (ScopeNode*)malloc(sizeof(ScopeNode));
    newNode->symbolTable = NULL;
    newNode->next = scopeStack;
    scopeStack = newNode;
}

void popScope() {
    if (scopeStack != NULL) {
        ScopeNode* tmp = scopeStack;
        scopeStack = scopeStack->next;
    } else {
        printf("Error: Attempted to pop empty scope stack\n");
    }
}

void insertSymbol(Symbol* s) {
    if (scopeStack != NULL) {
        HASH_ADD_STR(scopeStack->symbolTable, name, s);
    }
}

Symbol* findSymbol(const char* name) {
    Symbol* s = NULL;
    ScopeNode* currentNode = scopeStack;
    while (currentNode != NULL && s == NULL) {
        HASH_FIND_STR(currentNode->symbolTable, name, s);
        currentNode = currentNode->next;
    }
    return s;
}