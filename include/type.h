typedef struct Type{
    char name[20];
    enum {PRIMITIVE, ARRAY, STRUCTURE, FUNCTION} category;
    union{
        enum {INT, FLOAT, CHAR, BOOL} primitive;
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