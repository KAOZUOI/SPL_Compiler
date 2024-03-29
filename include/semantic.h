#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "APT.h"
#include "type.h"

void programSemaParser(Node node);
void extDefListSemaParser(Node node);
void extDefSemaParser(Node node);
void extDecListSemaParser(Node node, Type* type);

Type* specifierSemaParser(Node node);
Type* structSpecifierSemaParser(Node node);

FieldList* varDecSemaParser(Node node, Type* type);
Type* funDecSemaParser(Node node, Type* type);
FieldList* varListSemaParser(Node node, FieldList* fieldList);
FieldList* paramDecSemaParser(Node node, FieldList* fieldList);

void compStParser(Node node, Type* type);
void stmtListParser(Node node, Type* type);
void stmtParser(Node node, Type* type);


FieldList* defListSemaParser(Node node, FieldList* fieldList);
FieldList* defSemaParser(Node node, FieldList* fieldList);
FieldList* decListSemaParser(Node node, Type* type, FieldList* fieldList);
FieldList* decSemaParser(Node node, Type* type, FieldList* fieldList);

Type* expSemaParser(Node node);

int typeCmp(Type* typeA, Type* typeB);
#endif