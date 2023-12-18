#include "type.h"
#include <stdlib.h>

int deepcopyType(Type* dest, Type* sour){
    dest->space = sour->space;
    dest->tag = sour->tag;
    strcpy(dest->name, sour->name);
    dest->category = sour->category;
    switch (sour->category)
    {
        case PRIMITIVE:
            dest->primitive = sour->primitive;
            break;
        case ARRAY:
            dest->array = (Array*)malloc(sizeof(Array));
            deepcopyType(dest->array->type, sour->array->type);
            dest->array->size = sour->array->size;
            break;
        case STRUCTURE:
            dest->structure = (FieldList*)malloc(sizeof(FieldList));
            FieldList* destFieldList = dest->structure;
            FieldList* sourFieldList = sour->structure;
            while(sourFieldList){
                destFieldList->name = sourFieldList->name;
                destFieldList->type = (Type*)malloc(sizeof(Type));
                deepcopyType(destFieldList->type, sourFieldList->type);
                destFieldList->next = (FieldList*)malloc(sizeof(FieldList));
                destFieldList = destFieldList->next;
                sourFieldList = sourFieldList->next;
            }
            destFieldList->next = NULL;
            break;
        case FUNCTION:
            break;
        default:
            break;
    }
    
    return 1;
}
