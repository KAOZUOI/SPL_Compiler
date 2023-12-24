#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "APT.h"
#include "type.h"

typedef struct FuncParamLinkNode {
    char* tag;
    struct FuncParamLinkNode* prev;
    struct FuncParamLinkNode* next;
} FuncParamLinkNode;

void programSemaParser(Node node);
void extDefListSemaParser(Node node);
void extDefSemaParser(Node node);
void extDecListSemaParser(Node node, Type* type);

Type* specifierSemaParser(Node node);

FieldList* varDecSemaParser(int isStructDef, int isFuncParam, Node node, Type* type);
Type* funDecSemaParser(Node node, Type* type);
FieldList* varListSemaParser(Node node, FieldList* fieldList);
FieldList* paramDecSemaParser(Node node, FieldList* fieldList);

void compStParser(Node prev, Node node, Type* type);
void stmtListParser(Node prev, Node node, Type* type);
void stmtParser(Node prev, Node node, Type* type);


FieldList* defListSemaParser(int isStructDef, Node node, FieldList* fieldList, Type* structure, int totalSpace);
FieldList* defSemaParser(int isStructDef, Node node, FieldList* fieldList);
FieldList* decListSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList);
FieldList* decSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList);

Type* expSemaParser(int isAss, Node node);

int typeCmp(Type* typeA, Type* typeB);
void addReadFunc();
void addWriteFunc();
int countLength(int num);
#endif