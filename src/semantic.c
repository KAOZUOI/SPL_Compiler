#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "semantic.h"
#include "symbolTable.h"

extern FILE* fout;
uint32_t indent;

/*
Exp:  
        Exp ASSIGN Exp    
    |   Exp AND Exp    
    |   Exp OR Exp    
    |   Exp LT Exp    
    |   Exp LE Exp    
    |   Exp GT Exp    
    |   Exp GE Exp    
    |   Exp NE Exp    
    |   Exp EQ Exp    
    |   Exp PLUS Exp    
    |   Exp MINUS Exp    
    |   Exp MUL Exp    
    |   Exp DIV Exp    
    |   LP Exp RP    
    |   MINUS Exp    
    |   NOT Exp    
    |   ID LP Args RP    
    |   ID LP RP    
    |   Exp LB Exp RB    
    |   Exp DOT ID    
    |   ID    
    |   INT    
    |   HEX_INT    
    |   FLOAT    
    |   CHAR    
*/

Type* parseExp(Node exp) {
    // Exp:
    Node child = exp->left;
    Type* type = (Type*) malloc(sizeof(Type));
    if (!strcmp(child->name, "Exp")) {
        if (!strcmp(child->right->name, "ASSIGN")) {
    //     Exp ASSIGN Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!((!strcmp(exp1->left->name, "ID") && exp1->left->right == NULL) ||
                  (!strcmp(exp1->left->name, "Exp") && !strcmp(exp1->left->right->name, "DOT")) ||
                  (!strcmp(exp1->left->name, "Exp") && !strcmp(exp1->left->right->name, "LB")))) {
                fprintf(fout, "Error type 6 at Line %d: rvalue on the left side of assignment operator\n", child->lineno);
            } else if (!sameType(exp1_type, exp2_type)) {
                fprintf(fout, "Error type 5 at Line %d: unmatching types on both sides of assignment operator\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "AND") ||
                   !strcmp(child->right->name, "OR")) {
    // |   Exp AND Exp
    // |   Exp OR Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!sameType(exp1_type, exp2_type) &&
                exp1_type->category == PRIMITIVE &&
                exp1_type->primitive == INT) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "LT") ||
                   !strcmp(child->right->name, "LE") ||
                   !strcmp(child->right->name, "GT") ||
                   !strcmp(child->right->name, "GE") ||
                   !strcmp(child->right->name, "NE") ||
                   !strcmp(child->right->name, "EQ") ||
                   !strcmp(child->right->name, "PLUS") ||
                   !strcmp(child->right->name, "MINUS") ||
                   !strcmp(child->right->name, "MUL") ||
                   !strcmp(child->right->name, "DIV")) {
    // |   Exp LT Exp
    // |   Exp LE Exp
    // |   Exp GT Exp
    // |   Exp GE Exp
    // |   Exp NE Exp
    // |   Exp EQ Exp
    // |   Exp PLUS Exp
    // |   Exp MINUS Exp
    // |   Exp MUL Exp
    // |   Exp DIV Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!sameType(exp1_type, exp2_type) &&
                exp1_type->category == PRIMITIVE &&
                (exp1_type->primitive == INT || exp1_type->primitive == FLOAT)) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "LB")) {
    // |   Exp LB Exp RB
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
			if (exp1_type->category != ARRAY) {
				fprintf(fout, "Error type 10 at Line %d: applying indexing operator "
                              "on non-array type variables\n",
                        child->lineno);
			} else if (!(exp2_type->category == PRIMITIVE && exp2_type->primitive == INT)) {
                fprintf(fout, "Error type 12 at Line %d: array indexing with non-integer type expression\n",
                        child->lineno);
                type = exp1_type->array->type;
            } else {
                type = exp1_type->array->type;
            }
        } else if (!strcmp(child->right->name, "DOT")) {
    // |   Exp DOT ID
            Node exp1 = child;
            Node member_id = child->right->right;
            Type* exp1_type = parseExp(exp1);
            if (exp1_type->category != STRUCTURE) {
				fprintf(fout, "Error type 13 at Line %d: accessing member of non-structure variable\n",
                        child->lineno);
			} else {
                FieldList* field;
				for(field = exp1_type->structure; field != NULL; field = field->next) {
					if(!strcmp(field->name, member_id->string_value)) break;
                }
				if(field == NULL) {
					fprintf(fout, "Error type 14 at Line %d: accessing an undefined structure member \"%s\".\n",
                            member_id->lineno, member_id->string_value);
				} else {
					type = field->type;
				}
			}
        }
    } else if (!strcmp(child->name, "LP")) { 
    // |   LP Exp RP
        type = parseExp(child->right);
    } else if (!strcmp(child->name, "MINUS")) {
    // |   MINUS Exp
        Type* exp_type = parseExp(child->right);
        if(exp_type->category != PRIMITIVE) {
            fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
        } else {
            type = exp_type;
        }
    } else if (!strcmp(child->name, "NOT")) {
    // |   NOT Exp
        Type* exp_type = parseExp(child->right);
        if(!(exp_type->category == PRIMITIVE || exp_type->primitive == INT)) {
            fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
        } else {
            type->category = PRIMITIVE;
            type->primitive = INT;
        }
    } else if (!strcmp(child->name, "ID")) {
        Symbol* id_symbol = findSymbol(child->string_value);
        if (child->right != NULL) {
    // |   ID LP Args RP
    // |   ID LP RP
            if (id_symbol == NULL) {
				fprintf(fout, "Error type 1 at Line %d: function is invoked without definition \"%s\"\n",
                        child->lineno, child->string_value);
                return NULL;
			} else if (id_symbol->type->category != FUNCTION) {
                fprintf(fout, "Error type 11 at Line %d: applying function invocation "
                              "operator on non-function names \"%s\"\n",
                        child->lineno, child->string_value);
                return NULL;
            }
            Type* func_type = id_symbol->type;
            if (!strcmp(child->right->right->name, "Args")) {
                Node args = child->right->right;
                FieldList* args_type = func_type->structure->next;
                if (args_type == NULL) {
                    fprintf(fout, "Error type 9 at Line %d: the function "
                                  "%s’s arguments mismatch the declared parameters (too many)\n",
                                  child->lineno, child->string_value);
                } else {
                    // Args: Exp COMMA Args
                    //   |   Exp
                    Node exp = args->left;
                    while (1) {
                        Type* exp_type = parseExp(exp);
                        if (exp_type != NULL) {
                            if(!sameType(exp_type, args_type->type)) {
                                fprintf(fout, "Error type 9 at Line %d: the function "
                                        "%s’s arguments mismatch the declared parameters (type mismatch)\n",
                                        child->lineno, child->string_value);
                                break;
                            } else {
                                args_type = args_type->next;
                                if (args_type == NULL && exp->right == NULL) {
                                    type = func_type->structure->type;
                                    break;
                                } else if (args_type != NULL && exp->right == NULL) {
                                    fprintf(fout, "Error type 9 at Line %d: the function "
                                            "%s’s arguments mismatch the declared parameters (too few)\n",
                                            child->lineno, child->string_value);
                                    break;
                                } else if (args_type == NULL && exp->right != NULL) {
                                    fprintf(fout, "Error type 9 at Line %d: the function "
                                            "%s’s arguments mismatch the declared parameters (too many)\n",
                                            child->lineno, child->string_value);
                                    break;
                                }
                                exp = exp->right->right->left;
                            }
                        }
                    }
                }
            } else {
                if(func_type->structure->next == NULL) {
                    type = func_type->structure->type;
                } else {
                    fprintf(fout, "Error type 9 at Line %d: the function "
                                  "\"%s\"’s arguments mismatch the declared parameters (too few)\n",
                            child->lineno, child->string_value);
                }
            }
        } else {
    // |   ID
			if (id_symbol == NULL) {
				fprintf(fout, "Error type 1 at Line %d: variable is used without definition \"%s\".\n",
                        child->lineno, child->string_value);
			} else {
				type = id_symbol->type;
			}
        }
    } else if (!strcmp(child->name, "INT") ||
               !strcmp(child->name, "HEXINT")) {
    // |   INT
    // |   HEXINT
        type->category = PRIMITIVE;
		type->primitive = INT;
    } else if(!strcmp(child->name, "FLOAT")) {
    // |   FLOAT
        type->category = PRIMITIVE;
		type->primitive = FLOAT;
    } else if(!strcmp(child->name, "CHAR")) {
    // |   CHAR
        type->category = PRIMITIVE;
		type->primitive = CHAR;
    }
    return type;
}
    