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
FieldList* funDecSemaParser(Node node, Type* type);
FieldList* varListSemaParser(Node node);
FieldList* paramDecSemaParser(Node node);

#endif