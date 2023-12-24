#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tac.h"
#include "type.h"
#include "hashmap.h"
#include "translate.h"
#include "symbolTable.h"

Tac* head = NULL;
Tac* curTac = NULL;
char* onetag;
char* zerotag;
// used in if-else block
char* endTag;
int errorCnt, labelCnt, vCnt, tCnt, isEndTagDef;
char* NDtypes[] = { "TYPE", "INT", "FLOAT", "CHAR", "ID",
                    "STRUCT", "IF", "WHILE", "ELSE", "RETURN",
                    "BREAK", "CONTINUE", "DOT", "SEMI",
                    "COMMA", "ASSIGN", "QM", "COLON",
                    "LT", "LE", "GT", "GE", "NE", "EQ",
                    "AND", "OR", "NOT", "DMINUS",
                    "PLUS", "MINUS", "MUL", "DIV",
                    "LP", "RP", "LB", "RB", "LC", "RC",
                    "Program", "ExtDefList", "ExtDef", "Specifier",
                    "ExtDecList", "StructSpecifier",
                    "VarDec", "FunDec", "VarList", "ParamDec",
                    "CompSt", "StmtList", "Stmt", "DefList",
                    "Def", "DecList", "Dec", "Args", "Exp" };

extern FILE* fout;
int indent;

    /* Program */
void programSemaParser(Node node) {
    errorCnt = 0;
    labelCnt = 0;
    vCnt = 0;
    tCnt = 0;
    onetag = NULL;
    zerotag = NULL;
    endTag = NULL;
    isEndTagDef = 0;
    pushScope();
    addReadFunc();
    addWriteFunc();
    head = newTac(NULL, NULL, NULL, NULL);
    curTac = head;
    extDefListSemaParser(node->left);
    printTacs(head);
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
    if (node->left == NULL || node->left->right == NULL) return;
    Type* type = specifierSemaParser(node->left);
    if (strcmp(node->left->right->name, "ExtDecList") == 0) {
        extDecListSemaParser(node->left->right, type);
    } else if (strcmp(node->left->right->name, "FunDec") == 0) {
        Node funDec = node->left->right;
        Node compSt = node->left->right->right;
        Type* funDecType = funDecSemaParser(funDec, type);

        if(funDecType != NULL){
            Symbol* symbol = findSymbol(funDecType->structure->name);
            if(symbol != NULL){
                printf("Error type %d at Line %d: redefine the same function \"%s\".\n", 4, funDec->left->lineno, funDec->left->string_value);
            }else{
                symbol = (Symbol*)malloc(sizeof(Symbol));
                symbol->name = funDecType->structure->name;
                symbol->type = funDecType;
                symbol->offset = -1;
                insertSymbol(symbol);
                pushScope();
                compStParser(NULL, compSt, type);
                popScope();
            }
        }
    } else if (strcmp(node->left->right->name, "SEMI") == 0) {

    }
}
    /* ExtDecList */
void extDecListSemaParser(Node node, Type* type) {
    if (node->left->right == NULL) {
        varDecSemaParser(0, 0, node->left, type);
    } else {
        varDecSemaParser(0, 0, node->left, type);
        extDecListSemaParser(node->left->right->right, type);
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
            type->space = 4;
        } else if (strcmp(node->left->string_value, "float") == 0) {
            type->primitive = TYPE_FLOAT;
            type->space = 4;
        } else if (strcmp(node->left->string_value, "char") == 0) {
            type->primitive = TYPE_CHAR;
            type->space = 1;
        }
        return type;
    } else if (strcmp(node->left->name, "StructSpecifier") == 0) {
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
                pushScope();
                type->structure = defListSemaParser(1, defL, NULL, type, 0);
                popScope();
                // insert symbol
                symbol = (Symbol*)malloc(sizeof(Symbol));
                symbol->name = structId->string_value;
                symbol->type = type;
                printf("struct space %d\n", symbol->type->space);
                insertSymbol(symbol);
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
    return type;
}

    /* DefList */
FieldList* defListSemaParser(int isStructDef, Node node, FieldList* fieldList, Type* structure, int totalSpace) {
    // $
    if (!strcmp(node->name, "Epsilon")) {
        return fieldList;
    }
    // Def DefList
    if (fieldList == NULL) {
        fieldList = defSemaParser(isStructDef, node->left, fieldList);
        totalSpace = fieldList->type->space;
        if (isStructDef) {
            structure->space = totalSpace;
        }
    }else {
        fieldList->next = defSemaParser(isStructDef, node->left, fieldList);
        totalSpace += fieldList->next->type->space;
        if (isStructDef) {
            structure->space = totalSpace;
        }
        
    }
    defListSemaParser(isStructDef, node->left->right, fieldList, structure, totalSpace);
    return fieldList;
}


    /* Def */
FieldList* defSemaParser(int isStructDef, Node node, FieldList* fieldList) {
    Type* type = specifierSemaParser(node->left);
    fieldList = decListSemaParser(isStructDef, node->left->right, type, fieldList);
    return fieldList;
}



    /* DecList */
FieldList* decListSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList) {
    // Dec
    if (node->left->right == NULL) {
        fieldList = decSemaParser(isStructDef, node->left, type, fieldList);
    }else {
        // Dec COMMA DecList
        fieldList = decSemaParser(isStructDef, node->left, type, fieldList);
        decListSemaParser(isStructDef, node->left->right->right, type, fieldList);
    }
    return fieldList;
}
    /* Dec */
FieldList* decSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList) {
    //VarDec
    if (node->left->right == NULL) {
        FieldList* varDecField = varDecSemaParser(isStructDef, 0, node->left, type);
        return varDecField;
    }else {
        // VarDec ASSIGN Exp
        FieldList* varDecField = varDecSemaParser(isStructDef, 0, node->left, type);
        Type* expType = expSemaParser(0, node->left->right->right);
        if (!typeCmp(type, expType)) {
            printf("Error type %d at Line %d: unmatching types appear at both sides of the assignment operator (=) \n", 5, node->left->right->lineno);
        } else {
            Hashmap* item = findHashmap(varDecField->name);
            if (item != NULL) {
                curTac->next = newTac(item->value, NULL, expType->tag, NULL);
                curTac->next->title = ASS;
                curTac = curTac->next;
            }
        }
        return varDecField;
    }

}
    /* varDec */
FieldList* varDecSemaParser(int isStructDef, int isFuncParam, Node node, Type* type) {
    Type* oldType = (Type*)malloc(sizeof(Type));
    int success = deepcopyType(oldType, type);
    if (!success){
        free(oldType);
        oldType = NULL;
    }
    Node varDec = node->left;
    int spaces = 0;
    int unit = type->space;
    while (strcmp(varDec->name, "VarDec") == 0 && varDec->right != NULL) {
        Type* currentType = (Type*)malloc(sizeof(Type));
        currentType->array = (Array*)malloc(sizeof(Array));
        currentType->category = ARRAY;
        currentType->array->size = varDec->right->right->int_value;
        currentType->array->type = oldType;
        currentType->array->type->space = unit;
        spaces = unit * varDec->right->right->int_value;
        unit = spaces;
        oldType = currentType;
        varDec = varDec->left;
    }
    printf("space:%d\n", unit);

    oldType->space = unit;
    FieldList* fieldList = (FieldList*)malloc(sizeof(FieldList));
    fieldList->name = varDec->string_value; // ID->string_value
    fieldList->type = oldType;
    fieldList->next = NULL;
    Symbol* symbol = findSymbol(fieldList->name);
    if (symbol != NULL) {
        printf("Error type %d at Line %d: a variable is redefined in the same scope \"%s\".\n", 3, varDec->lineno, varDec->string_value);
    } else{
        if (!isStructDef){
            char* vtg = generateV(vCnt);
            vCnt++;
            if(isFuncParam){
                fieldList->type->tag = vtg;
                curTac->next = newTac(vtg, NULL, NULL, NULL);
                curTac = curTac->next;
                curTac->title = PARAM;
            } else{
                if(fieldList->type->category == ARRAY || fieldList->type->category == STRUCTURE){
                    int len = countLength(fieldList->type->space) + 1;
                    char* arg2 = (char*)malloc(sizeof(char) * len);
                    sprintf(arg2, "%d", fieldList->type->space);
                    fieldList->type->tag = (char*)malloc(sizeof(char)*(strlen(vtg)+1));
                    strcat(fieldList->type->tag, "&");
                    strcat(fieldList->type->tag, vtg);
                    curTac->next = newTac(NULL, NULL, vtg, arg2);
                    curTac->next->title = DEC;
                    curTac = curTac->next;
                }else{
                    fieldList->type->tag = vtg;
                }
            }
        }
        insertHashmap(fieldList->name, fieldList->type->tag);
        // printf("%s %s\n", fieldList->name, fieldList->type->tag);
    }
    // insert symbol
    symbol = (Symbol*)malloc(sizeof(Symbol));
    char* buffer = (char*) malloc(sizeof(char) * strlen(fieldList->name));
    strcpy(buffer, fieldList->name);
    symbol->name = buffer;
    symbol->type = fieldList->type;
    printf("True name:%s\n", symbol->name);
    printf("True space:%d\n", symbol->type->space);
    insertSymbol(symbol);
    return fieldList;
}
    /* varList */
FieldList* varListSemaParser(Node node, FieldList* fieldList) {
    // ParamDec COMMA VarList
    // ParamDec
    fieldList = paramDecSemaParser(node->left, fieldList);

    if (node->left->right != NULL) {
        fieldList->next = varListSemaParser(node->left->right->right, fieldList);
    }
    return fieldList;
}
    /* paramDec */
FieldList* paramDecSemaParser(Node node, FieldList* fieldList) {
    Type* type = specifierSemaParser(node->left);
    FieldList* varDecField = varDecSemaParser(0, 1, node->left->right, type);
    return varDecField;
}

    /* funDec */
Type* funDecSemaParser(Node node, Type* type) {
    Type* funDecType = (Type*)malloc(sizeof(Type));
    funDecType->category = FUNCTION;
    funDecType->structure = (FieldList*)malloc(sizeof(FieldList));
    funDecType->structure->type = type;
    funDecType->structure->name = node->left->string_value;
    FieldList* varListField = funDecType->structure;
    // check if node->left->right is NULL
    if (node->left->right == NULL) {
    }
    if (node->left->right->right == NULL) {
    }
    curTac->next = newTac(funDecType->structure->name, NULL, NULL, NULL);
    curTac->next->title = FUNC;
    curTac = curTac->next;
    // ID LP RP
    if (strcmp(node->left->right->right->name, "RP") == 0) {
        varListField->next = NULL;
        funDecType->structure->next = NULL;
    // ID LP VarList RP
    }else {
        funDecType->structure->next = varListSemaParser(node->left->right->right, varListField);
    }
    return funDecType;
}

Type* expSemaParser(int isAss, Node node){
    Type* type = NULL;

    if(strcmp(node->left->name, "Exp") == 0){
        if(strcmp(node->left->right->name, "ASSIGN") == 0){
            Type* expType = expSemaParser(0, node->left->right->right);
            // printf("expType: %s\n", expType->tag);
            Type* leftType = expSemaParser(1, node->left);
            if (!((strcmp(node->left->left->name, "ID") == 0 && node->left->left->right == NULL) || 
            (strcmp(node->left->left->name, "Exp") == 0 && strcmp(node->left->left->right->name, "DOT") == 0) || 
            (strcmp(node->left->left->name, "Exp") == 0 && strcmp(node->left->left->right->name, "LB") == 0))) {
                printf("Error type %d at Line %d: the left-hand side of an assignment must be a variable.\n", 6, node->left->lineno);
            } else if (expType != NULL && !typeCmp(leftType, expType)) {
                printf("Error type %d at Line %d: unmatching types appear at both sides of the assignment operator (=) \n", 5, node->left->lineno);
            } else {
                type = leftType;
                char* target = leftType->tag;
                Hashmap* item = findHashmap(target);
                if(item != NULL) {
                    target = item->value;
                }
                if (!strcmp(curTac->target, expType->tag)) {
                    if(target[0] == '*'){
                        curTac->next = newTac(target, NULL, expType->tag, NULL);
                        curTac->next->title = ASS;
                        curTac = curTac->next;
                    } else {
                        curTac->target = target;
                    }
                } else {
                    curTac->next = newTac(target, NULL, expType->tag, NULL);
                    curTac->next->title = ASS;
                    curTac = curTac->next;
                } 

            }
        } else if(strcmp(node->left->right->name, "AND") == 0 || strcmp(node->left->right->name, "OR") == 0){

            Type* expType1 = expSemaParser(0, node->left);
            Type* expType2 = expSemaParser(0, node->left->right->right);

            if(typeCmp(expType1, expType2) == 0 && expType1->category != PRIMITIVE && expType1->primitive != TYPE_INT){
                printf("Error type %d at Line %d: the type of operands of logical operator must be integer.\n", 7, node->left->lineno);
            }else{
                type = (Type*)malloc(sizeof(Type));
                type->category = PRIMITIVE;
                type->primitive = TYPE_INT;
                int isAnd = !strcmp(node->left->right->name,"AND")? 1 : 0;
                if (isAnd){
                    int otagll = labelCnt;
                    int ztagl = labelCnt + 1;
                    int otagrl = labelCnt + 2;
                    labelCnt += 3;
                    char* tmpTag = generateLabel(otagll);
                    char* otag = generateLabel(otagrl);
                    char* ztag = generateLabel(ztagl);
                    curTac->next = newTac(tmpTag, NULL, expType1->tag, NULL);
                    curTac = curTac->next; curTac->title = IF;
                    curTac->next = newTac(ztag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = GOTO;
                    curTac->next = newTac(tmpTag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = LABEL;
                    curTac->next = newTac(otag, NULL, expType1->tag, NULL);
                    curTac = curTac->next; curTac->title = IF;
                    curTac->next = newTac(ztag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = GOTO;
                    if(onetag != NULL) free(onetag);
                    if(zerotag != NULL) free(zerotag);
                    onetag = (char*)malloc(sizeof(char) * strlen(otag));
                    strcpy(onetag, otag);
                    zerotag = (char*)malloc(sizeof(char) * strlen(ztag));
                    strcpy(zerotag, ztag);
                }else{
                    int otagl = labelCnt;
                    int ztagll = labelCnt + 1;
                    int ztagrl = labelCnt + 2;
                    labelCnt += 3;
                    char* tmpTag = generateLabel(ztagll);
                    char* otag = generateLabel(otagl);
                    char* ztag = generateLabel(ztagrl);
                    curTac->next = newTac(otag, NULL, expType1->tag, NULL);
                    curTac = curTac->next; curTac->title = IF;
                    curTac->next = newTac(tmpTag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = GOTO;
                    curTac->next = newTac(tmpTag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = LABEL;
                    curTac->next = newTac(otag, NULL, expType1->tag, NULL);
                    curTac = curTac->next; curTac->title = IF;
                    curTac->next = newTac(ztag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = GOTO;
                    if(onetag != NULL) free(onetag);
                    if(zerotag != NULL) free(zerotag);
                    onetag = (char*)malloc(sizeof(char) * strlen(otag));
                    strcpy(onetag, otag);
                    zerotag = (char*)malloc(sizeof(char) * strlen(ztag));
                    strcpy(zerotag, ztag);
                }
            }
        } else if(strcmp(node->left->right->name, "LT") == 0 ||
        strcmp(node->left->right->name, "LE") == 0 ||
        strcmp(node->left->right->name, "GT") == 0 ||
        strcmp(node->left->right->name, "GE") == 0 ||
        strcmp(node->left->right->name, "NE") == 0 ||
        strcmp(node->left->right->name, "EQ") == 0) {
            Type* expType1 = expSemaParser(0, node->left);

            Type* expType2 = expSemaParser(0, node->left->right->right);

            if((typeCmp(expType1, expType2) == 0) && (expType1->primitive == TYPE_INT || expType1->primitive == TYPE_FLOAT)){
                printf("Error type %d at Line %d: the type of operands of relational operator must be integer.\n", 7, node->left->lineno);
            }else{
                type = (Type*)malloc(sizeof(Type));
                type->category = PRIMITIVE;
                type->primitive = TYPE_INT;
                int len = strlen(expType1->tag) + strlen(expType2->tag);
                char* oper = NULL;
                if(!strcmp(node->left->right->name,"LT")){ len += 3; oper = " < ";
                }else if (!strcmp(node->left->right->name,"LE")){ len += 4; oper = " <= ";
                }else if (!strcmp(node->left->right->name,"GT")){ len += 3; oper = " > ";
                }else if (!strcmp(node->left->right->name,"GE")){ len += 4; oper = " >= ";
                }else if (!strcmp(node->left->right->name,"NE")){ len += 4; oper = " != ";
                }else{ len += 4; oper = " == "; }
                type->tag = (char*)malloc(sizeof(char) * len);
                strcat(type->tag, expType1->tag);
                strcat(type->tag, oper);
                strcat(type->tag, expType2->tag);
            }

        }else if(strcmp(node->left->right->name, "PLUS") == 0 ||
        strcmp(node->left->right->name, "MINUS") == 0 ||
        strcmp(node->left->right->name, "MUL") == 0 ||
        strcmp(node->left->right->name, "DIV") == 0) {

            Type* expType1 = expSemaParser(0, node->left);

            Type* expType2 = expSemaParser(0, node->left->right->right);

            if((typeCmp(expType1, expType2) == 0) && (expType1->primitive == TYPE_INT || expType1->primitive == TYPE_FLOAT)){
                printf("Error type %d at Line %d: the type of operands of relational operator must be integer.\n", 7, node->left->lineno);
            }else{
                type = (Type*)malloc(sizeof(Type));
                type->category = PRIMITIVE;
                if(expType1->primitive == TYPE_FLOAT || expType2->primitive == TYPE_FLOAT){
                    type->primitive = TYPE_FLOAT;
                }else{
                    type->primitive = TYPE_INT;
                }
                char* irop = NULL;
                if (!strcmp(node->left->right->name,"PLUS")){
                    irop = "+";
                }else if (!strcmp(node->left->right->name,"MINUS")){
                    irop = "-";
                }else if (!strcmp(node->left->right->name,"MUL")){
                    irop = "*";
                }else{
                    irop = "/";
                }
                if (expType1->tag[0] == '#' && expType2->tag[0] == '#') {
                    char lf[10] = {0};
                    char rg[10] = {0};
                    for (size_t i = 1; i < strlen(expType1->tag); i++) {
                        lf[i - 1] = expType1->tag[i];
                    }
                    for (size_t i = 1; i < strlen(expType2->tag); i++) {
                        rg[i - 1] = expType2->tag[i];
                    }
                    int left = atoi(lf);
                    int right = atoi(rg);
                    int res = 0;
                    if (irop[0] == '+') res = left + right;
                    else if (irop[0] == '-') res = left - right;
                    else if (irop[0] == '*') res = left * right;
                    else res = left / right;
                    int len = countLength(res) + 1;
                    char tmp[10] = {0};
                    sprintf(tmp, "%d", res);
                    type->tag = (char*)malloc(sizeof(char) * len);
                    strcat(type->tag, "#");
                    strcat(type->tag, tmp);
                }else{
                    if (irop[0] == '*'){
                        if (!strcmp(expType2->tag, "#1")){
                            type->tag = (char*)malloc(sizeof(char)*strlen(expType1->tag));
                            strcpy(type->tag, expType1->tag);
                            return type;
                        }else if (!strcmp(expType1->tag, "#1")){
                            type->tag = (char*)malloc(sizeof(char)*strlen(expType2->tag));
                            strcpy(type->tag, expType2->tag);
                            return type;
                        }
                    }else if (irop[0] == '/'){
                        if (!strcmp(expType2->tag, "#1")){
                            type->tag = (char*)malloc(sizeof(char)*strlen(expType1->tag));
                            strcpy(type->tag, expType1->tag);
                            return type;
                        }
                    }else if (irop[0] == '-' && !strcmp(expType1->tag, expType2->tag)){
                        type->tag = "#0";
                        return type;
                    }
                    type->tag = generateT(tCnt);
                    curTac-> next = newTac(type->tag, irop, expType1->tag, expType2->tag);
                    curTac = curTac->next;
                    curTac->title = OPER;
                    tCnt++;
                }
            }

        } else if(strcmp(node->left->right->name, "LB") == 0){

            Type* expType1 = expSemaParser(isAss, node->left);
            Type* expType2 = expSemaParser(isAss, node->left->right->right);
            if(expType1->category != ARRAY){
                printf("Error type %d at Line %d: the type of operands of \"[]\" must be an array.\n", 10, node->left->lineno);
            }else if(expType2->category != PRIMITIVE || expType2->primitive != TYPE_INT){
                printf("Error type %d at Line %d: the type of operands of \"[]\" must be an integer.\n", 12, node->left->lineno);
                type = expType1->array->type;
            }else{
                type = (Type*)malloc(sizeof(Type));

                deepcopyType(type, expType1->array->type);
                char* addr = NULL;
                if ((expType2->tag[0] == '#' && expType2->tag[1] != '0')
                || expType2->tag[0] == 'v'){
                    int unit = expType1->array->type->space;
                    char* spc = NULL;
                    char num[10] = {0};
                    char mrk[10] = "#";
                    int len = countLength(unit) + 1;
                    sprintf(num, "%d", unit);
                    strcat(mrk, num);
                    char* ost = (char*)malloc(sizeof(char) * len);
                    strncpy(ost, mrk, len);
                    if (expType2->tag[0] == '#' 
                    && expType2->tag[1] == '1'
                    && expType2->tag[2] == '\0'){
                        spc = ost;
                    }else{
                        // t? := unit * INT
                        spc = generateT(tCnt);
                        tCnt++;
                        curTac->next = newTac(spc, "*", expType2->tag, ost);
                        curTac = curTac->next;
                        curTac->title = OPER;
                    }
                    // t? := addr + spc
                    addr = generateT(tCnt);
                    tCnt++;
                    curTac->next = newTac(addr, "+", expType1->tag, spc);
                    curTac = curTac->next;
                    curTac->title = OPER;
                }else{
                    addr = expType1->tag;
                }
                // t? := *addr
                if(type->category == ARRAY || type->category == STRUCTURE){
                    type->tag = addr;
                }else{
                    type->tag = generateT(tCnt);
                    tCnt++;
                    char* val = (char*)malloc(sizeof(char)*(strlen(addr) + 1));
                    strcat(val, "*");
                    strcat(val, addr);
                    insertHashmap(type->tag, val);
                    if (!isAss) {
                        curTac->next = newTac(type->tag, NULL, val, NULL);
                        curTac = curTac->next;
                        curTac->title = ASS;
                    }
                }
            }

        } else if(strcmp(node->left->right->name, "DOT") == 0){
            Type* expType1 = expSemaParser(isAss, node->left);
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
                    FieldList* tmp = expType1->structure;
                    int isError = 0;
                    int offset = 0;
                    while(tmp != NULL){
                        if(!strcmp(tmp->name, node->left->right->right->string_value)){
                            isError = 1;
                            type = (Type*)malloc(sizeof(Type));
                            deepcopyType(type, tmp->type);
                            char* addr = NULL;
                            if (tmp->type->tag == NULL || strlen(tmp->type->tag) == 0){
                                if (offset > 0){
                                    char num[10] = {0};
                                    char mrk[10] = "#";
                                    int len = countLength(offset) + 1;
                                    sprintf(num, "%d", offset);
                                    strcat(mrk, num);
                                    char* ost = (char*)malloc(sizeof(char)*len);
                                    strncpy(ost, mrk, len);
                                    // t? := addr + offset;
                                    addr = generateT(tCnt);
                                    tCnt++;
                                    curTac->next = newTac(addr, "+", expType1->tag, ost);
                                    curTac = curTac->next;
                                    curTac->title = OPER;
                                }else if (expType1->tag[0] == '&'){
                                    // t? := &v?;
                                    addr = generateT(tCnt);
                                    tCnt++;
                                    curTac->next = newTac(addr, NULL, expType1->tag, NULL);
                                    curTac = curTac->next;
                                    curTac->title = ASS;
                                }else{
                                    // v?;
                                    addr = expType1->tag;
                                }
                                // t? := *addr
                                if (tmp->type->category == STRUCTURE || tmp->type->category == ARRAY){
                                    type->tag = addr;
                                }else{
                                    type->tag = generateT(tCnt);
                                    tCnt++;
                                    char* val = (char*)malloc(sizeof(char)*(strlen(addr) + 1));
                                    strcat(val, "*");
                                    strcat(val, addr);
                                    insertHashmap(type->tag, val);
                                    if (!isAss) {
                                        curTac->next = newTac(type->tag, NULL, val, NULL);
                                        curTac = curTac->next;
                                        curTac->title = ASS;
                                    }
                                }
                            }
                            break;
                        }
                        // offset += tmp->type->dec;
                        offset += tmp->type->space;
                        tmp = tmp->next;
                    }
                    if (!isError){
                        printf("Error type 14 at Line %d: no such member: %s\n", 
                        node->left->lineno, node->left->right->right->string_value);
                        errorCnt++;
                    }
                }
            }
        }
    } else if(strcmp(node->left->name, "LP") == 0){

        type = expSemaParser(0, node->left->right);
        type = type;
    } else if(strcmp(node->left->name, "MINUS") == 0){ // TODO

        Type* expType1 = expSemaParser(0, node->left->right);
        if(expType1->category != PRIMITIVE){
            printf("Error type %d at Line %d: the type of operands of unary operator \"-\" must be an integer or a float.\n", 7, node->left->lineno);
        }else{
            type = (Type*)malloc(sizeof(Type));
            type->category = PRIMITIVE;
            type->primitive = expType1->primitive;
            char* key = (char*)malloc(sizeof(char) * (strlen(expType1->tag) + 3));
            strcat(key, "#0");
            strcat(key, "-");
            strcat(key, expType1->tag);
            Hashmap* item = findHashmap(key);
            if (item == NULL){
                type->tag = generateT(tCnt);
                curTac-> next = newTac(type->tag, "-", "#0", expType1->tag);
                curTac = curTac->next;
                curTac->title = OPER;
                insertHashmap(key, type->tag);
                tCnt++;
            }else{
                type->tag = item->value;
            }
        }
    } else if(strcmp(node->left->name, "NOT") == 0){

        Type* expType1 = expSemaParser(0, node->left->right);
        if(expType1->category != PRIMITIVE || expType1->primitive != TYPE_INT){
            printf("Error type %d at Line %d: the type of operands of unary operator \"!\" must be an integer.\n", 7, node->left->lineno);
        }else{
            type = (Type*)malloc(sizeof(Type));
            type->category = PRIMITIVE;
            type->primitive = TYPE_INT;
            int len = strlen(expType1->tag);
            type->tag = (char*)malloc(sizeof(char) * len);
            for (size_t i = 0; i < len; i++){
                if (!strncmp(expType1->tag + i, "!=", 2)){
                    type->tag[i] = '=';
                    type->tag[i + 1] = '=';
                    i++;
                }else if (!strncmp(expType1->tag + i, "==", 2)){
                    type->tag[i] = '!';
                    type->tag[i + 1] = '=';
                    i++;
                }else if (!strncmp(expType1->tag + i, ">", 1)){
                    type->tag[i] = '<';
                }else if (!strncmp(expType1->tag + i, "<", 1)){
                    type->tag[i] = '>';
                }else{
                    type->tag[i] = expType1->tag[i];
                }
            }
        }
    } else if(strcmp(node->left->name, "ID") == 0){
        Symbol* symbol = findSymbol(node->left->string_value);
        printf("symbol name %s\n", node->left->string_value);
        printf("symbol type %d\n", symbol->type->category);
        if(node->left->right != NULL){
            if(symbol == NULL){
                printf("Error type %d at Line %d: a function is invoked without a definition \"%s\".\n", 1, node->left->lineno, node->left->string_value);
                return NULL;
            }else if(symbol->type == NULL || symbol->type->category != FUNCTION){
                printf("Error type %d at Line %d: \"%s\" is not a function.\n", 11, node->left->lineno, node->left->string_value);
                return NULL;
            }
            Type* funDecType = symbol->type;
            // printf("funDecType->structure->name %s\n", funDecType->structure->name);
            
            if(strcmp(node->left->right->right->name, "Args") == 0){
                Node args = node->left->right->right;
                FieldList* fieldList = funDecType->structure->next;
                if(fieldList == NULL){
                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                }else{
                    Node argsExp = args->left;
                    // printf("argsExp->left->name %s\n", argsExp->left->name);
                    // printf("argsExp->left->string_value %s\n", argsExp->left->string_value);
                    FuncParamLinkNode* fir = (FuncParamLinkNode*)malloc(sizeof(FuncParamLinkNode));
                    fir->prev = NULL;
                    fir->next = NULL;
                    while(1){
                        // printf("while(1) in\n");
                        Type* expType = expSemaParser(0, argsExp);
                        // printf("while(1) out\n");
                        if(expType == NULL){
                            break;
                        } else {
                            if(!typeCmp(fieldList->type, expType)){
                                printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                break;
                            } else {
                                fir->tag = (char*)malloc(sizeof(char) * strlen(expType->tag));
                                strncpy(fir->tag, expType->tag, strlen(expType->tag));
                                fir->next = (FuncParamLinkNode*)malloc(sizeof(FuncParamLinkNode));
                                FuncParamLinkNode* tmp = fir;
                                fir = fir->next;
                                fir->prev = tmp;
                                fieldList = fieldList->next;
                                if (fieldList == NULL && argsExp->right == NULL){
                                    type = (Type*)malloc(sizeof(Type));
                                    int success = deepcopyType(type, funDecType->structure->type);
                                    if (!success){
                                        free(type);
                                        type = NULL;
                                    }
                                    fir = fir->prev;
                                    break;
                                }else if(fieldList == NULL && argsExp->right != NULL){
                                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                    errorCnt++;
                                    break;
                                }else if(fieldList == NULL && argsExp->right != NULL){
                                    printf("Error type %d at Line %d: the function \"%s\" is not applicable for arguments.\n", 9, node->left->lineno, node->left->string_value);
                                    errorCnt++;
                                    break;
                                }
                                // args = args->left->right->right; //caide
                                argsExp = argsExp->right->right->left; //caide
                            }
                        }
                    }// TODO
                    if(!strcmp(funDecType->structure->name, "write")){
                        curTac->next = newTac(fir->tag, NULL, NULL, NULL);
                        curTac = curTac->next; curTac->title = WRITE;
                    }else{
                        while (fir != NULL){
                            curTac->next = newTac(fir->tag, NULL, NULL, NULL);
                            curTac = curTac->next; curTac->title = ARG;
                            fir = fir->prev;
                        }
                        type->tag = generateT(tCnt);
                        int calllen = strlen(funDecType->structure->name) + 5;
                        char* arg1 = (char*)malloc(sizeof(char)*calllen);
                        strcat(arg1, "CALL ");
                        strcat(arg1, funDecType->structure->name);
                        curTac->next = newTac(type->tag, NULL, arg1, NULL);
                        curTac = curTac->next;
                        curTac->title = ASS;
                        tCnt++;
                    }

                }
            } else {
                if(funDecType->structure->next == NULL){
                    type = (Type*)malloc(sizeof(Type));
                    int success = deepcopyType(type, funDecType->structure->type);
                    if (!success){
                        free(type);
                        type = NULL;
                    }
                    if (!strcmp(funDecType->structure->name, "read")){
                        type->tag = generateT(tCnt);
                        curTac->next = newTac(type->tag, NULL, NULL, NULL);
                        curTac = curTac->next;
                        curTac->title = READ;
                        tCnt++;
                    }
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
        char num[10] = "#";
        // strcat(num, node->left->int_value);
        sprintf(num, "#%d", node->left->int_value);
        type = (Type*)malloc(sizeof(Type));
        type->category = PRIMITIVE;
        type->primitive = TYPE_INT;
        type->tag = (char*)malloc(sizeof(char) * (countLength(node->left->int_value) + 1));
        strncpy(type->tag, num, (countLength(node->left->int_value) + 1));
    } else if(strcmp(node->left->name, "FLOAT") == 0){
        type = (Type*)malloc(sizeof(Type));
        type->category = PRIMITIVE;
        type->primitive = TYPE_FLOAT;
    } else if(strcmp(node->left->name, "CHAR") == 0){
        type = (Type*)malloc(sizeof(Type));
        type->category = PRIMITIVE;
        type->primitive = TYPE_CHAR;
    }
    return type;
}

void compStParser(Node prev, Node node, Type* type){
    defListSemaParser(0, node->left->right, NULL, NULL, 0);
    stmtListParser(prev, node->left->right->right, type);
}

void stmtListParser(Node prev, Node node, Type* type) {
    if (node->left != NULL) {
        stmtParser(prev, node->left, type);
        stmtListParser(prev, node->left->right, type);
    }

}

void stmtParser(Node prev, Node node, Type* type){

    if(strcmp(node->left->name, "Exp") == 0){
        expSemaParser(0, node->left);
    }else if(strcmp(node->left->name, "CompSt") == 0){
        compStParser(prev, node->left, type);
    }else if(strcmp(node->left->name, "RETURN") == 0){
        Type* expType = expSemaParser(0, node->left->right);
        if (expType != NULL && !typeCmp(type, expType)) {
            printf("Error type %d at Line %d: a function's return value type mismatches the declared type \n", 8, node->left->lineno);
        } else {
            curTac->next = newTac(expType->tag, NULL, NULL, NULL);
            curTac->next->title = RETURN;
            curTac = curTac->next;
        }
    } else if(strcmp(node->left->name, "IF") == 0){
        Node expIf = node->left->right->right;
        Type* expType = expSemaParser(0, expIf);
        //only int variables can do boolean operations
        if(expType != NULL){
            if (expType->category != PRIMITIVE || expType->primitive != TYPE_INT) {
                printf("Error type %d at Line %d: the condition expression of \"if\" statement must be an integer.\n", 7, node->left->right->lineno);
            } else {
                char* trueTag = NULL;
                char* falseTag = NULL;
                if(expType->tag != NULL && strlen(expType->tag) > 0){
                    int falseLabel = labelCnt;
                    labelCnt += 2;
                    falseTag = generateLabel(falseLabel);
                    // tag convert
                    if (expType->tag[3] == '!') {
                        expType->tag[3] = '=';
                        expType->tag[4] = '=';
                    } else if (expType->tag[3] == '=') {
                        expType->tag[3] = '!';
                        expType->tag[4] = '=';
                    } else if (expType->tag[3] == '>' && expType->tag[4] == '=') {
                        expType->tag[3] = '<';
                        for (int i = 4; i < strlen(expType->tag); i++) {
                            expType->tag[i - 1] = expType->tag[i];
                        }
                    } else if (expType->tag[3] == '<' && expType->tag[4] == '=') {
                        expType->tag[3] = '>';
                        for (int i = 4; i < strlen(expType->tag); i++) {
                            expType->tag[i - 1] = expType->tag[i];
                        }
                    } else if (expType->tag[3] == '>' && expType->tag[4] != '=') {
                        expType->tag[3] = '<';
                        for (int i = strlen(expType->tag); i > 3; i--) {
                            expType->tag[i] = expType->tag[i - 1];
                        }
                        expType->tag[4] = '=';
                    } else if (expType->tag[3] == '<' && expType->tag[4] != '=') {
                        expType->tag[3] = '>';
                        for (int i = strlen(expType->tag); i > 3; i--) {
                            expType->tag[i] = expType->tag[i - 1];
                        }
                        expType->tag[4] = '=';
                    }
                    printf ("expType->tag: %s\n", expType->tag);
                    curTac->next = newTac(falseTag, NULL, expType->tag, NULL);
                    curTac->next->title = IF;
                    curTac = curTac->next;
                    // // false condition
                    // falseTag = generateLabel(falseLabel);
                    // curTac->next = newTac(falseTag, NULL, NULL, NULL);
                    // curTac->next->title = GOTO;
                    // curTac = curTac->next;
                    // // true label
                    // curTac->next = newTac(trueTag, NULL, NULL, NULL);
                    // curTac->next->title = LABEL;
                    // curTac = curTac->next;
                    pushScope();
                    stmtParser(prev, expIf->right->right, type);
                    popScope();
                }else{
                    //true label
                    trueTag = (char*)malloc(sizeof(char) * strlen(onetag));
                    falseTag = (char*)malloc(sizeof(char) * strlen(zerotag));
                    strcpy(trueTag, onetag);
                    strcpy(falseTag, zerotag);
                    curTac->next = newTac(trueTag, NULL, NULL, NULL);
                    curTac = curTac->next; curTac->title = LABEL;
                    pushScope();
                    stmtParser(prev, expIf->right->right, type);
                    popScope();
                }
                if(expIf->right->right->right != NULL){
                    if(!isEndTagDef){
                        endTag = generateLabel(labelCnt++);
                        isEndTagDef++;
                    }
                    if(curTac->title != RETURN){
                        curTac->next = newTac(endTag, NULL, NULL, NULL);
                        curTac->next->title = GOTO;
                        curTac = curTac->next;
                    }
                    // false label
                    curTac->next = newTac(falseTag, NULL, NULL, NULL);
                    curTac->next->title = LABEL;
                    curTac = curTac->next;
                    pushScope();
                    stmtParser(prev, expIf->right->right->right->right, type);
                    popScope();
                    if(isEndTagDef){
                        isEndTagDef--;
                        if(curTac->title != RETURN){
                            curTac->next = newTac(endTag, NULL, NULL, NULL);
                            curTac->next->title = LABEL;
                            curTac = curTac->next;
                        }
                    }
                } else {
                    // false label
                    curTac->next = newTac(falseTag, NULL, NULL, NULL);
                    curTac->next->title = LABEL;
                    curTac = curTac->next;
                }
            }
        }
    } else if(strcmp(node->left->name, "WHILE") == 0){
        Node expWhile = node->left->right->right;
        int firstLabel = labelCnt;
        int trueLabel = labelCnt + 1;
        int falseLabel = labelCnt + 2;
        int len = 0;
        labelCnt += 3;
        char* firstTag = generateLabel(firstLabel);
        curTac->next = newTac(firstTag, NULL, NULL, NULL);
        curTac->next->title = LABEL;
        curTac = curTac->next;
        Type* expType = expSemaParser(0, expWhile);
        //only int variables can do boolean operations
        if(expType == NULL) return;
        if (expType->category != PRIMITIVE || expType->primitive != TYPE_INT) {
            printf("Error type %d at Line %d: the condition expression of \"while\" statement must be an integer.\n", 7, expWhile->lineno);
        } 
        if(expType->tag != NULL && strlen(expType->tag) > 0){
            // true condition
            char* trueTag = generateLabel(trueLabel);
            curTac->next = newTac(trueTag, NULL, expType->tag, NULL);
            curTac->next->title = IF;
            curTac = curTac->next;
            // false condition
            char* falseTag = generateLabel(falseLabel);
            curTac->next = newTac(falseTag, NULL, NULL, NULL);
            curTac->next->title = GOTO;
            curTac = curTac->next;
            // true label
            curTac->next = newTac(trueTag, NULL, NULL, NULL);
            curTac->next->title = LABEL;
            curTac = curTac->next;
            pushScope();
            stmtParser(prev, expWhile->right->right, type);
            curTac->next = newTac(firstTag, NULL, NULL, NULL);
            curTac->next->title = GOTO;
            curTac = curTac->next;
            // false label
            curTac->next = newTac(falseTag, NULL, NULL, NULL);
            curTac->next->title = LABEL;
            curTac = curTac->next;
            popScope();
        } else {
             //true label
            char* otag = (char*)malloc(sizeof(char) * strlen(onetag));
            char* ztag = (char*)malloc(sizeof(char) * strlen(zerotag));
            strcpy(otag, onetag);
            strcpy(ztag, zerotag);
            curTac->next = newTac(otag, NULL, NULL, NULL);
            curTac = curTac->next; curTac->title = LABEL;
            pushScope();
            stmtParser(prev, expWhile->right->right, type);
            curTac->next = newTac(firstTag, NULL, NULL, NULL);
            curTac = curTac->next; curTac->title = GOTO;
            //false label
            curTac->next = newTac(ztag, NULL, NULL, NULL);
            curTac = curTac->next; curTac->title = LABEL;
            popScope();
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

void addReadFunc(){
    Type* readFunc = (Type*)malloc(sizeof(Type));
    readFunc->category = FUNCTION;
    readFunc->structure = (FieldList*)malloc(sizeof(FieldList));
    readFunc->structure->name = "read";
    readFunc->structure->type = (Type*)malloc(sizeof(Type));
    readFunc->structure->type->category = PRIMITIVE;
    readFunc->structure->type->primitive = TYPE_INT;
    readFunc->structure->next = NULL;
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->name = "read";
    symbol->type = readFunc;
    insertSymbol(symbol);
}

void addWriteFunc(){
    Type* writeFunc = (Type*)malloc(sizeof(Type));
    writeFunc->category = FUNCTION;
    writeFunc->structure = (FieldList*)malloc(sizeof(FieldList));
    writeFunc->structure->name = "write";
    writeFunc->structure->type = NULL;
    writeFunc->structure->next = (FieldList*)malloc(sizeof(FieldList));
    writeFunc->structure->next->next = NULL;
    writeFunc->structure->next->type = (Type*)malloc(sizeof(Type));
    writeFunc->structure->next->type->category = PRIMITIVE;
    writeFunc->structure->next->type->primitive = TYPE_INT;
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->name = "write";
    symbol->type = writeFunc;
    insertSymbol(symbol);
}

int countLength(int num){
    int cnt = 0;
    while (num > 0)
    {
        cnt++;
        num /= 10;
    }
    return cnt == 0 ? 1 : cnt;
}
