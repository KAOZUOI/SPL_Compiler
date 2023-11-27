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
    extDefListSemaParser(node->left->right);
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
        return type;
    } else if (strcmp(node->left->name, "StructSpecifier") == 0) {

        Node structId = node->left->right;
        Symbol* symbol = findSymbol(structId->string_value);
        // Struct ID LC DefList RC
        if (structId->right != NULL) {
            if (symbol != NULL) {
                printf("Error type %d at Line %d: redefine the same structure type \"%s\".\n", 15, node->left->right->lineno, node->left->right->string_value);
                indent = indent - 2;
                return NULL;
            }
            type = (Type*)malloc(sizeof(Type));
            type->category = STRUCTURE;
            // DefList
            Node defL = structId->right->right;
            type->structure = defListSemaParser(defL, NULL);
            // insert symbol
            symbol = (Symbol*)malloc(sizeof(Symbol));
            symbol->name = structId->string_value;
            symbol->type = type;
            insertSymbol(symbol);
        }else {
            // Struct ID
            // TODO: check if the struct is defined and if the id is a struct
            type = (Type*)malloc(sizeof(Type));
            type = symbol->type;
        }
    }
    return type;
}

    /* DefList */
FieldList* defListSemaParser(Node node, FieldList* fieldList) {
    // $
    if (!strcmp(node->name, "Epsilon")) {
        return fieldList;
    }
    // Def DefList
    if (fieldList == NULL) {
        fieldList = defSemaParser(node->left, fieldList);
    }else {
        fieldList->next = defSemaParser(node->left, fieldList);
    }
    defListSemaParser(node->right, fieldList);
    return fieldList;
}

FieldList* defSemaParser(Node node, FieldList* fieldList) {
    Type* type = specifierSemaParser(node->left);
    fieldList = decListSemaParser(node->left->right, type, fieldList);
    return fieldList;
}

    /* DecList */
FieldList* decListSemaParser(Node node, Type* type, FieldList* fieldList) {
    // Dec
    if (node->left->right == NULL) {
        fieldList = decSemaParser(node->left, type, fieldList);
    }else {
        // Dec COMMA DecList
        fieldList = decSemaParser(node->left, type, fieldList);
        decListSemaParser(node->left->right->right, type, fieldList);
    }
    return fieldList;
}

FieldList* decSemaParser(Node node, Type* type, FieldList* fieldList) {
    // VarDec
    if (node->left->right == NULL) {
        // TODO
        // fieldList = varDecSemaParser(node->left, type);
        
    }else {
        // VarDec ASSIGN Exp
        // Type* expType = expSemaParser(node->left->right->right);
        // TODO
        
        
    }
    return fieldList;
}

static uint32_t typeCmp(Type* type1, Type* type2) {
    // TODO
    if (type1->category == PRIMITIVE && type2->category == PRIMITIVE) {
        if (type1->primitive == type2->primitive) {
            return 1;
        }else {
            return 0;
        }
    }else if (type1->category == STRUCTURE && type2->category == STRUCTURE) {
        if (strcmp(type1->structure->name, type2->structure->name) == 0) {
            return 1;
        }else {
            return 0;
        }
    }else {
        return 0;
    }
}

    /* VarDec */