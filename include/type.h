#ifndef TYPE_H
#define TYPE_H
typedef struct Type{
    int space;
    char* tag;
    char name[20];
    enum {PRIMITIVE, ARRAY, STRUCTURE, FUNCTION} category;
    union{
        enum {TYPE_INT, TYPE_FLOAT, TYPE_CHAR} primitive;
        struct Array* array;
        struct FieldList* structure;
    };

} Type;

typedef struct Array{
    struct Type* type;
    int size;
} Array;

typedef struct FieldList{
    char* name;
    struct Type* type;
    struct FieldList* next;
} FieldList;

int deepcopyType(Type* dest, Type* sour);

#endif