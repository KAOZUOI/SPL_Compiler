#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "semantic.h"
#include "symbolTable.h"

extern FILE* fout;
int indent;

    /* Program */
void programSemaParser(Node node) {
    pushScope();
    extDefListSemaParser(node->left);
    popScope();
}
    /* ExtDefList */
void extDefListSemaParser(Node node) {
    if (node->left == NULL) return;
    extDefSemaParser(node->left);
    extDefListSemaParser(node->left->right);
}
    /* ExtDef */
void extDefSemaParser(Node node) {
    #ifdef DEBUG
    printf("ExtDef_in\n");
    #endif
    if (node->left == NULL || node->left->right == NULL) return;
    Type* type = specifierSemaParser(node->left);
    #ifdef DEBUG
    printf("Specifier_inExtDef\n");
    #endif
    if (strcmp(node->left->right->name, "ExtDecList") == 0) {
        extDecListSemaParser(node->left->right, type);
        #ifdef DEBUG
        printf("ExtDecList_inExtDef\n");
        #endif
    } else if (strcmp(node->left->right->name, "FunDec") == 0) {
        Node funDec = node->left->right;
        Node compSt = node->left->right->right;
        #ifdef DEBUG
        printf("FunDec_inExtDef\n");
        printf("CompSt_inExtDef\n");
        #endif
        Type* funDecType = funDecSemaParser(funDec, type);
        #ifdef DEBUG
        printf("FunDecType_inExtDef\n");
        #endif

        if(funDecType != NULL){
            #ifdef DEBUG
            printf("if_inExtDef\n");
            #endif
            Symbol* symbol = findSymbol(funDecType->structure->name);
            if(symbol != NULL){
                printf("Error type %d at Line %d: redefine the same function \"%s\".\n", 4, funDec->left->lineno, funDec->left->string_value);
            }else{
                #ifdef DEBUG
                printf("else_inExtDef\n");
                #endif
                symbol = (Symbol*)malloc(sizeof(Symbol));
                symbol->name = funDecType->structure->name;
                symbol->type = funDecType;
                insertSymbol(symbol);
                #ifdef DEBUG
                printf("insertSymbol_inExtDef:\n");
                printf("name: %s\n", symbol->name);
                printf("type: %s\n", symbol->type->name);
                #endif
                pushScope();
                compStParser(compSt, type);
                popScope();
            }
        }
    } else if (strcmp(node->left->right->name, "SEMI") == 0) {

    }
}
    /* ExtDecList */
void extDecListSemaParser(Node node, Type* type) {
    if (node->left->right == NULL) {
        varDecSemaParser(node->left, type);
    } else {
        varDecSemaParser(node->left, type);
        extDecListSemaParser(node->left->right->right, type);
    }
}

    /* Specifier */
Type* specifierSemaParser(Node node) {
    Type* type = NULL;
    #ifdef DEBUG
    printf("Specifier_in \n");
    if (node->left == NULL) printf("Specifier_left: NULL\n");
    else printf("Specifier_left: %s\n", node->left->name);
    #endif
    if (strcmp(node->left->name, "TYPE") == 0) {
        #ifdef DEBUG
        printf("Specifier_inTYPE \n");
        #endif
        type = (Type*)malloc(sizeof(Type));
        type->category = PRIMITIVE;
        if (strcmp(node->left->string_value, "int") == 0) {
            type->primitive = TYPE_INT;
            #ifdef DEBUG
            printf("Int \n");
            #endif
        } else if (strcmp(node->left->string_value, "float") == 0) {
            type->primitive = TYPE_FLOAT;
            #ifdef DEBUG
            printf("Float \n");
            #endif
        } else if (strcmp(node->left->string_value, "char") == 0) {
            type->primitive = TYPE_CHAR;
            #ifdef DEBUG
            printf("Char \n");
            #endif
        }
        return type;
    } else if (strcmp(node->left->name, "StructSpecifier") == 0) {
        #ifdef DEBUG
        printf("Specifier_inStructSpecifier \n");
        #endif
        Node structId = node->left->left->right;
            Symbol* symbol = findSymbol(structId->string_value);
            // Struct ID LC DefList RC
            if (structId->right != NULL) {
                if (symbol != NULL) {
                    printf("Error type %d at Line %d: redefine the same structure type \"%s\".\n", 15, node->left->left->right->lineno, node->left->left->right->string_value);
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
                #ifdef DEBUG
                printf("insertSymbol_inStructSpecifier:\n");
                printf("name: %s\n", symbol->name);
                printf("type: %s\n", symbol->type->name);
                #endif
            }else {
                // Struct ID
                type = (Type*)malloc(sizeof(Type));
                if (symbol == NULL) {
                    printf("Error type %d at Line %d: undefined structure type \"%s\".\n", 17, node->left->right->lineno, node->left->right->string_value);
                    return NULL;
                }
                if(symbol->type->category != STRUCTURE) {
                    printf("Error type %d at Line %d: \"%s\" is not a structure.\n", 16, node->left->right->lineno, node->left->right->string_value);
                    return NULL;
                }
                type = symbol->type;
            }
    }
    #ifdef DEBUG
    printf("Specifier_out \n");
    #endif
    return type;
}

    /* DefList */
FieldList* defListSemaParser(Node node, FieldList* fieldList) {
    // $
    #ifdef DEBUG
    printf("node->name: %s\n", node->name);
    #endif
    if (!strcmp(node->name, "Epsilon")) {
        return fieldList;
    }
    // Def DefList
    if (fieldList == NULL) {
        #ifdef DEBUG
        printf("Def_inDefList\n");
        #endif
        fieldList = defSemaParser(node->left, fieldList);
        #ifdef DEBUG
        printf("Def_outDefList\n");
        #endif
    }else {
        #ifdef DEBUG
        printf("Def_next_inDefList: %s\n", node->left->name);
        #endif

        //print fieldList

        fieldList->next = defSemaParser(node->left, fieldList);

        #ifdef DEBUG
        printf("Def_next_outDefList\n");
        #endif

    }
    #ifdef DEBUG
    printf("DefList_inDefList\n");
    #endif
    defListSemaParser(node->left->right, fieldList);
    #ifdef DEBUG
    printf("DefList_outDefList\n");
    #endif
    return fieldList;
}
    /* Def */
FieldList* defSemaParser(Node node, FieldList* fieldList) {
    #ifdef DEBUG
    printf("Specifier_inDef \n");
    #endif
    Type* type = specifierSemaParser(node->left);
    #ifdef DEBUG
    printf("decList_inDef\n");
    #endif
    fieldList = decListSemaParser(node->left->right, type, fieldList);
    #ifdef DEBUG
    printf("decList_outDef");
    #endif
    return fieldList;
}

    /* DecList */
FieldList* decListSemaParser(Node node, Type* type, FieldList* fieldList) {
    // Dec
    if (node->left->right == NULL) {
        #ifdef DEBUG
        printf("Dec_inDecList_up\n");
        #endif

        fieldList = decSemaParser(node->left, type, fieldList);
        #ifdef DEBUG
        printf("Dec_outDecList_up\n");
        #endif
    }else {
        // Dec COMMA DecList
        #ifdef DEBUG
        printf("Dec_inDecList\n");
        #endif
        fieldList = decSemaParser(node->left, type, fieldList);
        #ifdef DEBUG
        printf("Dec_outDecList\n");
        printf("DecList_inDecList\n");
        #endif
        decListSemaParser(node->left->right->right, type, fieldList);
        #ifdef DEBUG
        printf("DecList_outDecList\n");
        #endif
    }
    return fieldList;
}
    /* Dec */
FieldList* decSemaParser(Node node, Type* type, FieldList* fieldList) {
    //VarDec
    if (node->left->right == NULL) {
        #ifdef DEBUG
        printf("VarDec_inDec_up\n");
        #endif
        FieldList* varDecField = varDecSemaParser(node->left, type);
        #ifdef DEBUG
        printf("VarDec_outDec_up\n");
        #endif
        return varDecField;
    }else {
        // VarDec ASSIGN Exp
        #ifdef DEBUG
        printf("VarDec_inDec 00000\n");
        #endif
        FieldList* varDecField = varDecSemaParser(node->left, type);
        Type* expType = expSemaParser(node->left->right->right);
        #ifdef DEBUG
        printf("VarDec_outDec\n");
        #endif
        if (!typeCmp(type, expType)) {
            printf("Error type %d at Line %d: unmatching types appear at both sides of the assignment operator (=) \n", 5, node->left->right->lineno);
        }
        return varDecField;
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
        printf("Error type %d at Line %d: a variable is redefined in the same scope \"%s\".\n", 3, varDec->lineno, varDec->string_value);
    }
    // insert symbol
    symbol = (Symbol*)malloc(sizeof(Symbol));
    char* buffer = (char*) malloc(sizeof(char) * strlen(fieldList->name));
    strcpy(buffer, fieldList->name);
    symbol->name = buffer;
    symbol->type = fieldList->type;
    insertSymbol(symbol);
    #ifdef DEBUG
    printf("insertSymbol_inVarDec:\n");
    printf("name: %s\n", symbol->name);
    printf("type: %s\n", symbol->type->name);
    #endif
    return fieldList;
}
    /* varList */
FieldList* varListSemaParser(Node node, FieldList* fieldList) {
    // ParamDec COMMA VarList
    if (node->left->right != NULL) {
        #ifdef DEBUG
        printf("ParamDec_inVarList\n");
        #endif
        fieldList = paramDecSemaParser(node->left, fieldList);
        #ifdef DEBUG
        printf("ParamDec_outVarList\n");
        printf("VarList_inVarList\n");
        #endif
        fieldList = varListSemaParser(node->left->right->right, fieldList);
        #ifdef DEBUG
        printf("VarList_outVarList\n");

        #endif
        return fieldList;
    }else {
        // ParamDec
        #ifdef DEBUG
        printf("ParamDec_inVarList\n");
        #endif
        fieldList = paramDecSemaParser(node->left, fieldList);
        #ifdef DEBUG
        printf("ParamDec_outVarList\n");
        #endif
        return fieldList;
    }
}
    /* paramDec */
FieldList* paramDecSemaParser(Node node, FieldList* fieldList) {
    #ifdef DEBUG
    printf("Specifier_inParamDec\n");
    #endif
    Type* type = specifierSemaParser(node->left);
    #ifdef DEBUG
    printf("Dec_inParamDec\n");
    #endif
    FieldList* varDecField = varDecSemaParser(node->left->right, type);
    #ifdef DEBUG
    printf("Dec_outParamDec,FieldList = %s\n", varDecField->name);
    #endif
    return varDecField;
}

    /* funDec */
Type* funDecSemaParser(Node node, Type* type) {
    Type* funDecType = (Type*)malloc(sizeof(Type));
    funDecType->category = FUNCTION;
    funDecType->structure = (FieldList*)malloc(sizeof(FieldList));
    funDecType->structure->type = type;
    funDecType->structure->name = node->left->string_value;
    #ifdef DEBUG
    printf("FunDec\n");
    #endif
    FieldList* varListField = funDecType->structure;
    // check if node->left->right is NULL
    if (node->left->right == NULL) {
        #ifdef DEBUG
        printf("NULL1\n");
        #endif
    }
    if (node->left->right->right == NULL) {
        #ifdef DEBUG
        printf("NULL2\n");
        #endif
    }
    // ID LP RP
    if (strcmp(node->left->right->right->name, "RP") == 0) {
        varListField->next = NULL;
        #ifdef DEBUG
        printf("RP_inFunDec\n");
        #endif
    // ID LP VarList RP
    }else {
        #ifdef DEBUG
        printf("VarList_inFunDec\n");
        #endif
        varListSemaParser(node->left->right->right, varListField);
        #ifdef DEBUG
        printf("VarList_outFunDec\n");
        #endif
    }
    return funDecType;
}

Type* expSemaParser(Node node){
    Type* type = malloc(sizeof(Type));
    #ifdef DEBUG
    printf("Exp\n");
    printf("Exp_left\n");
    #endif
    // printf("Exp_right: %s\n", node->left->right->name);
    if(strcmp(node->left->name, "Exp") == 0){
        #ifdef DEBUG
        printf("Exp_in\n");
        #endif
        if(strcmp(node->left->right->name, "ASSIGN") == 0){
            #ifdef DEBUG
            printf("ASSIGN_in\n");
            #endif
            Type* expType = expSemaParser(node->left->right->right);
            #ifdef DEBUG
            printf("ASSIGN_expType\n");
            #endif
            Type* leftType = expSemaParser(node->left);
            #ifdef DEBUG
            printf("ASSIGN_leftType\n");
            #endif
            if (!((strcmp(node->left->left->name, "ID") == 0 && node->left->left->right == NULL) || 
            (strcmp(node->left->left->name, "Exp") == 0 && strcmp(node->left->left->right->name, "DOT") == 0) || 
            (strcmp(node->left->left->name, "Exp") == 0 && strcmp(node->left->left->right->name, "LB") == 0))) {
                printf("Error type %d at Line %d: the left-hand side of an assignment must be a variable.\n", 6, node->left->lineno);
            } else if (expType != NULL && !typeCmp(leftType, expType)) {
                printf("Error type %d at Line %d: unmatching types appear at both sides of the assignment operator (=) \n", 5, node->left->lineno);
            } else {
                type = leftType;
            }
            #ifdef DEBUG
            printf("ASSIGN_out\n");
            #endif
        } else if(strcmp(node->left->right->name, "AND") == 0 || strcmp(node->left->right->name, "OR") == 0){
            #ifdef DEBUG
            printf("AND_OR_in\n");
            #endif
            Type* expType1 = expSemaParser(node->left);
            Type* expType2 = expSemaParser(node->left->right->right);
            #ifdef DEBUG
            printf("AND_OR_expType1\n");
            #endif
            if(typeCmp(expType1, expType2) == 0 && expType1->category != PRIMITIVE && expType1->primitive != TYPE_INT){
                printf("Error type %d at Line %d: the type of operands of logical operator must be integer.\n", 7, node->left->lineno);
            }else{
                type = expType1;
            }
            #ifdef DEBUG
            printf("AND_OR_out\n");
            #endif
        } else if(strcmp(node->left->right->name, "LT") == 0 ||
        strcmp(node->left->right->name, "LE") == 0 ||
        strcmp(node->left->right->name, "GT") == 0 ||
        strcmp(node->left->right->name, "GE") == 0 ||
        strcmp(node->left->right->name, "NE") == 0 ||
        strcmp(node->left->right->name, "EQ") == 0 ||
        strcmp(node->left->right->name, "PLUS") == 0 ||
        strcmp(node->left->right->name, "MINUS") == 0 ||
        strcmp(node->left->right->name, "MUL") == 0 ||
        strcmp(node->left->right->name, "DIV") == 0) {
            #ifdef DEBUG
            printf("LT_GE_in\n");
            #endif
            Type* expType1 = expSemaParser(node->left);
            #ifdef DEBUG
            printf("LT_GE_expType1\n");
            #endif
            Type* expType2 = expSemaParser(node->left->right->right);
            #ifdef DEBUG
            printf("LT_GE_expType2\n");
            #endif
            if((typeCmp(expType1, expType2) == 0) && (expType1->primitive == TYPE_INT || expType1->primitive == TYPE_FLOAT)){
                printf("Error type %d at Line %d: the type of operands of relational operator must be integer.\n", 7, node->left->lineno);
            }else{
                type = expType1;
            }
            #ifdef DEBUG
            printf("LT_GE_out\n");
            #endif
        } else if(strcmp(node->left->right->name, "LB") == 0){
            #ifdef DEBUG
            printf("LB_in\n");
            #endif
            Type* expType1 = expSemaParser(node->left);
            Type* expType2 = expSemaParser(node->left->right->right);
            if(expType1->category != ARRAY){
                printf("Error type %d at Line %d: the type of operands of \"[]\" must be an array.\n", 10, node->left->lineno);
            }else if(expType2->category != PRIMITIVE || expType2->primitive != TYPE_INT){
                printf("Error type %d at Line %d: the type of operands of \"[]\" must be an integer.\n", 12, node->left->lineno);
                type = expType1->array->type;
            }else{
                type = expType1->array->type;
            }
            #ifdef DEBUG
            printf("LB_out\n");
            #endif
        } else if(strcmp(node->left->right->name, "DOT") == 0){
            #ifdef DEBUG
            printf("DOT_in\n");
            #endif
            Type* expType1 = expSemaParser(node->left);
            if(expType1->category != STRUCTURE){
                printf("Error type %d at Line %d: the type of operands of \".\" must be a structure.\n", 13, node->left->lineno);
            }else{
                FieldList* fieldList;
                for(fieldList = expType1->structure; fieldList != NULL; fieldList = fieldList->next){
                    if(strcmp(fieldList->name, node->left->right->right->string_value) == 0){
                        break;
                    }
                }
                if(fieldList == NULL){
                    printf("Error type %d at Line %d: the structure does not have the field \"%s\".\n", 14, node->left->lineno, node->left->right->right->string_value);
                }else{
                    type = fieldList->type;
                }
            }
            #ifdef DEBUG
            printf("DOT_out\n");
            #endif
        }
    } else if(strcmp(node->left->name, "LP") == 0){
        #ifdef DEBUG
        printf("LP_in\n");
        #endif
        type = expSemaParser(node->left->right);
    } else if(strcmp(node->left->name, "MINUS") == 0){
        #ifdef DEBUG
        printf("MINUS_in\n");
        #endif
        Type* expType1 = expSemaParser(node->left->right);
        if(expType1->category != PRIMITIVE){
            printf("Error type %d at Line %d: the type of operands of unary operator \"-\" must be an integer or a float.\n", 7, node->left->lineno);
        }else{
            type = expType1;
        }
    } else if(strcmp(node->left->name, "NOT") == 0){
        #ifdef DEBUG
        printf("NOT_in\n");
        #endif
        Type* expType1 = expSemaParser(node->left->right);
        if(expType1->category != PRIMITIVE || expType1->primitive != TYPE_INT){
            printf("Error type %d at Line %d: the type of operands of unary operator \"!\" must be an integer.\n", 7, node->left->lineno);
        }else{
            type->category = PRIMITIVE;
            type->primitive = TYPE_INT;
        }
    } else if(strcmp(node->left->name, "ID") == 0){
        #ifdef DEBUG
        printf("ID_in\n");
        #endif
        Symbol* symbol = findSymbol(node->left->string_value);
        if(node->left->right != NULL){
            #ifdef DEBUG
            printf("node->left->right: %s\n", node->left->right->name);
            #endif
            if(symbol == NULL){
                printf("Error type %d at Line %d: a function is invoked without a definition \"%s\".\n", 1, node->left->lineno, node->left->string_value);
                return NULL;
            }else if(symbol->type->category != FUNCTION){
                printf("Error type %d at Line %d: \"%s\" is not a function.\n", 11, node->left->lineno, node->left->string_value);
                return NULL;
            }
            Type* funDecType = symbol->type;
            if(strcmp(node->left->right->right->name, "Args") == 0){
                Node args = node->left->right->right;
                FieldList* fieldList = funDecType->structure->next;
                if(fieldList == NULL){
                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                }else{
                    Node exp = args->left;
                    while(1){
                        Type* expType = expSemaParser(exp);
                        if(expType != NULL){
                            if(!typeCmp(fieldList->type, expType)){
                                printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                break;
                            } else {
                                fieldList = fieldList->next;
                                if(fieldList == NULL && exp->right == NULL){
                                    type = funDecType->structure->type;
                                    break;
                                } else if(fieldList == NULL && exp->right != NULL){
                                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                    break;
                                } else if(fieldList != NULL && exp->right == NULL){
                                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                    break;
                                }
                                exp = exp->right->right->left;
                            }
                        }

                    }
                }
            } else {
                if(funDecType->structure->next == NULL){
                    type = funDecType->structure->type;
                }else{
                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                }
            } 
        } else{
            if(symbol == NULL){
                printf("Error type %d at Line %d: undefined variable \"%s\".\n", 1, node->left->lineno, node->left->string_value);
            }else{
                type = symbol->type;
            }
        }
    } else if(strcmp(node->left->name, "INT") == 0){
        #ifdef DEBUG
        printf("INT_in\n");
        #endif
        type->category = PRIMITIVE;
        type->primitive = TYPE_INT;
    } else if(strcmp(node->left->name, "FLOAT") == 0){
        #ifdef DEBUG
        printf("FLOAT_in\n");
        #endif
        type->category = PRIMITIVE;
        type->primitive = TYPE_FLOAT;
    } else if(strcmp(node->left->name, "CHAR") == 0){
        #ifdef DEBUG
        printf("CHAR_in\n");
        #endif
        type->category = PRIMITIVE;
        type->primitive = TYPE_CHAR;
    }
    return type;
}

void compStParser(Node node, Type* type){
    #ifdef DEBUG
    printf("CompSt");
    #endif
    defListSemaParser(node->left->right, NULL);
    stmtListParser(node->left->right->right, type);
    #ifdef DEBUG
    printf("StmtList_inCompSt");
    #endif
}

void stmtListParser(Node node, Type* type) {
    if (node->left != NULL) {
        stmtParser(node->left, type);
        stmtListParser(node->left->right, type);
    }

}

void stmtParser(Node node, Type* type){

    if(strcmp(node->left->name, "Exp") == 0){
        expSemaParser(node->left);
    }else if(strcmp(node->left->name, "CompSt") == 0){
        pushScope();
        compStParser(node->left, type);
        popScope();
    }else if(strcmp(node->left->name, "RETURN") == 0){
        Type* expType = expSemaParser(node->left->right);
        if (expType != NULL && !typeCmp(type, expType)) {
            printf("Error type %d at Line %d: a function's return value type mismatches the declared type \n", 8, node->left->lineno);
        }
    } else if(strcmp(node->left->name, "IF") == 0){
        Node expIf = node->left->right->right;
        Type* expType = expSemaParser(expIf);
        //only int variables can do boolean operations
        if(expType != NULL){
            if (expType->category != PRIMITIVE || expType->primitive != TYPE_INT) {
                printf("Error type %d at Line %d: the condition expression of \"if\" statement must be an integer.\n", 7, node->left->right->lineno);
            } else {
                stmtParser(expIf->right->right, type);
                if(expIf->right->right->right != NULL){
                    stmtParser(expIf->right->right->right->right, type);
                }
            }
        }
    } else if(strcmp(node->left->name, "WHILE") == 0){
        Node expWhile = node->left->right->right;
        Type* expType = expSemaParser(expWhile);
        //only int variables can do boolean operations
        if(expType != NULL){
            if (expType->category != PRIMITIVE || expType->primitive != TYPE_INT) {
                printf("Error type %d at Line %d: the condition expression of \"while\" statement must be an integer.\n", 7, expWhile->lineno);
            } else {
                stmtParser(expWhile->right->right, type);
            }
        }
    }
}


    /* Static Function*/
int typeCmp(Type* typeA, Type* typeB) {
    if (!typeA || !typeB) return 0;

    if (typeA->category == PRIMITIVE) {
            if(typeB->category == PRIMITIVE && typeA->primitive == typeB->primitive){
                return 1;
            }
    }
    else if (typeA->category == ARRAY){
            if (typeB->category == ARRAY && 
                    typeA->array->size == typeB->array->size && 
                    typeCmp(typeA->array->type, typeB->array->type)){
                        return 1;
                    }
    }
    else if (typeA->category == STRUCTURE || typeA->category == FUNCTION) {
        if (typeB->category != typeA->category) return 0;

            FieldList* fieldListA = typeA->structure;
            FieldList* fieldListB = typeB->structure;

            while(fieldListA != NULL && fieldListB != NULL) {
				if(!typeCmp(fieldListA->type, fieldListB->type)) {
					return 0;
                }
				fieldListA = fieldListA->next;
				fieldListB = fieldListB->next;
			}
			if(fieldListA != NULL || fieldListB != NULL) {
				return 0;
            }
			return 1;
    }
    return 0;
}
    // if (typeA == NULL || typeB == NULL) {
    //     return 0;
    // }
    // if (typeA->category == typeB->category) {
    //     if (typeA->category == PRIMITIVE && typeA->primitive == typeB->primitive) {
    //         return 1;
    //     } else if (typeA->category == ARRAY &&
    //         typeCmp(typeA->array->type, typeB->array->type) &&
    //         typeA->array->size == typeB->array->size) {
    //         return 1;
    //     } else if(typeA->category == STRUCTURE || typeA->category == FUNCTION) {
	// 		FieldList* field1 = typeA->structure;
	// 		FieldList* field2 = typeB->structure;
	// 		while(field1 != NULL && field2 != NULL)
	// 		{
	// 			if(!typeCmp(field1->type, field2->type)) {
	// 				return 0;
    //             }
	// 			field1 = field1->next;
	// 			field2 = field2->next;
	// 		}
	// 		if(field1 != NULL || field2 != NULL) {
	// 			return 0;
    //         }
	// 		return 1;
	// 	}
    // }



