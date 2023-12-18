#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "APT.h"
#include "type.h"

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


FieldList* defListSemaParser(int isStructDef, Node node, FieldList* fieldList);
FieldList* defSemaParser(int isStructDef, Node node, FieldList* fieldList);
FieldList* decListSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList);
FieldList* decSemaParser(int isStructDef, Node node, Type* type, FieldList* fieldList);

Type* expSemaParser(int isAss, Node node);

int typeCmp(Type* typeA, Type* typeB);
#endif