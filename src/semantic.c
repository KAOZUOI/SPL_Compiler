#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "semantic.h"
#include "symbolTable.h"

extern FILE* fout;
uint32_t indent;

    /* Program */
void programSemaParser(Node node) {
    extDefListSemaParser(node->left);
}

void extDefListSemaParser(Node node) {
    if (node->left == NULL) return;
    extDefSemaParser(node->left);
    extDefListSemaParser(node->right->right);
}

void extDefSemaParser(Node node) {
    Type* type = specifierSemaParser(node->left);
    if (strcmp(node->left->right->name, "ExtDecList") == 0) {
        extDecListSemaParser(node->left->right, type);
    } else if (strcmp(node->left->right->name, "FunDec") == 0) {
        // TODO FunDec
    } else if (strcmp(node->left->right->name, "SEMI") == 0) {
        // TODO SEMI
    }
}

void extDecListSemaParser(Node node, Type* type) {
    if (node->left->right == NULL) {
        // TODO varDec
    } else {
        // TODO varDec COMMA extDecList
    }
}

    /* Specifier */
Type* specifierSemaParser(Node node) {
    Type* type = NULL;
    if (strcmp(node->left->name, "TYPE") == 0) {
        type = (Type*)malloc(sizeof(Type));
        type->category = PRIMITIVE;
        if (strcmp(node->left->string_value, "int") == 0) {
            type->primitive = TYPE_INT;
            printf("Int \n");
        } else if (strcmp(node->left->string_value, "float") == 0) {
            type->primitive = TYPE_FLOAT;
            printf("Float \n");
        } else if (strcmp(node->left->string_value, "char") == 0) {
            type->primitive = TYPE_CHAR;
            printf("Char \n");
        }
        // TODO Bool
        return type;
    } else if (strcmp(node->left->name, "StructSpecifier") == 0) {
        // TODO StructSpecifier
    }
    return type;
}
