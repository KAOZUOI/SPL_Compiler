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
    /* ExtDefList */
void extDefListSemaParser(Node node) {
    if (node->left == NULL) return;
    extDefSemaParser(node->left);
    extDefListSemaParser(node->left->right);
}
    /* ExtDef */
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
    /* ExtDecList */
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
    /* Def */
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
    /* Dec */
FieldList* decSemaParser(Node node, Type* type, FieldList* fieldList) {
    // VarDec
    if (node->left->right == NULL) {
        FieldList* varDecField = varDecSemaParser(node->left, type);
        return varDecField;
    }else {
        // VarDec ASSIGN Exp
        Type* expType = expSemaParser(node->left->right->right);
        if (!typeCmp(type, expType)) {
            printf("Error type %d at Line %d: unmatching types appear at both sides of the assignment operator (=) \n", 5, node->left->right->lineno);
            indent = indent - 2;
            return NULL;
        }
    }
}
    /* varDec */
FieldList* varDecSemaParser(Node node, Type* type) {
    Type* oldType = type;
    Node varDec = node->left;
    while (strcmp(varDec->name, "VarDec") == 0) {
        Type* currentType = (Type*)malloc(sizeof(Type));
        currentType->array = (Array*)malloc(sizeof(Array));
        currentType->category = ARRAY;
        currentType->array->size = varDec->right->right->int_value;
        currentType->array->type = oldType;
        oldType = currentType;
        varDec = varDec->left;
    }

    FieldList* fieldList = (FieldList*)malloc(sizeof(FieldList));
    fieldList->name = varDec->string_value; // ID->string_value
    fieldList->type = oldType;
    fieldList->next = NULL;
    Symbol* symbol = findSymbol(fieldList->name);
    if (symbol != NULL) {
        printf("Error type %d at Line %d: a variable is redefined in the same scope \"%s\".\n", 3, varDec->lineno, varDec->name);
    }
    // insert symbol
    symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->name = fieldList->name;
    symbol->type = fieldList->type;
    insertSymbol(symbol);
    return fieldList;
}
    /* varList */
FieldList* varListSemaParser(Node node, FieldList* fieldList) {
    // ParamDec COMMA VarList
    if (node->left->right != NULL) {
        FieldList* paramDecField = paramDecSemaParser(node->left, fieldList);
        FieldList* varListField = varListSemaParser(node->left->right->right, fieldList);
        return paramDecField;
    }else {
        // ParamDec
        FieldList* paramDecField = paramDecSemaParser(node->left, fieldList);
        return paramDecField;
    }
}
    /* paramDec */
FieldList* paramDecSemaParser(Node node, FieldList* fieldList) {
    Type* type = specifierSemaParser(node->left);
    FieldList* varDecField = varDecSemaParser(node->left->right, type);
    return varDecField;
}

    /* funDec */
FieldList* funDecSemaParser(Node node, Type* type) {
    Type* funDecType = (Type*)malloc(sizeof(Type));
    funDecType->category = FUNCTION;
    funDecType->structure = (FieldList*)malloc(sizeof(FieldList));
    funDecType->structure->type = type;
    funDecType->structure->name = node->left->string_value;
    FieldList* varListField = funDecType->structure;
    // ID LP RP
    if (strcmp(node->left->right->right->name, "RP") == 0) {
        varListField->next = NULL;
    // ID LP VarList RP
    }else {
        FieldList* fieldList = varListSemaParser(node->left->right->right, varListField);
    }
    return type;
}

// TODO: Exp
// TODO: CompSt
// TODO: StmtList
// TODO: Stmt




    /* Static Function*/
static uint32_t typeCmp(Type* typeA, Type* typeB) {
    if (!typeA || !typeB) return 0;

    switch (typeA->category) {
        case PRIMITIVE:
            return (typeB->category == PRIMITIVE && typeA->primitive == typeB->primitive);

        case ARRAY:
            return (typeB->category == ARRAY && 
                    typeA->array->size == typeB->array->size && 
                    typeCmp(typeA->array->type, typeB->array->type));

        case STRUCTURE:
        case FUNCTION:
            if (typeB->category != typeA->category) return 0;

            FieldList* fieldListA = typeA->structure;
            FieldList* fieldListB = typeB->structure;

            while (fieldListA && fieldListB) {
                if (!typeCmp(fieldListA->type, fieldListB->type)) return 0;
                fieldListA = fieldListA->next;
                fieldListB = fieldListB->next;
            }

            return (fieldListA == NULL && fieldListB == NULL);

        default:
            return 0;
    }
}


