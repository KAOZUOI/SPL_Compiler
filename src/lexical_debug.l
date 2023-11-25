%{
    #include "src/APT.h"
    #include "syntax_debug.tab.h"

    #define __ASSIGN_LEAF_NODE(NAME)    { \
        yylval.AptNode = newAnnotatedParseLeafNode(#NAME, yylineno); \
        return NAME; \
    }
    
    extern FILE* fout;
%}
%option yylineno

%x IN_COMMENT
    /* keywords */
TYPE    "int"|"float"|"char"
STRUCT  "struct"
IF      "if"
ELSE    "else"
WHILE   "while"
RETURN  "return"
DOT     "."
SEMI    ";"
COMMA   ","
ASSIGN  "="
LT      "<"
LE      "<="
GT      ">"
GE      ">="
NE      "!="
EQ      "=="
PLUS    "+"
MINUS   "-"
MUL     "*"
DIV     "/"
AND     "&&"
OR      "||"
NOT     "!"
LP      "("
RP      ")"
LB      "["
RB      "]"
LC      "{"
RC      "}"

    /* integer in 32-bits (decimal or hexadecimal) */
INT                 [+-]?[0-9]+
HEX_INT             [+-]?0x[0-9a-fA-F]+
WRONG_HEX_INT       [+-]?0x[0-9a-zA-Z]+

    /* floating point number (only dot-form) */
FLOAT               [-+]?(([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+))

    /* single character (printable or hex-form) */
CHAR                '(([ -~])|(\\x[0-9a-fA-F]{2}))'
WRONG_HEX_CHAR      '\\x[0-9a-zA-Z]*'
ID                  [_a-zA-Z][_a-zA-Z0-9]*
WRONG_ID            [0-9][_a-zA-Z0-9]*|[_a-zA-Z0-9]*&[^&][_a-zA-Z0-9]*|&{3,}|[_a-zA-Z0-9]*[%$]+[_a-zA-Z0-9]*|[_a-zA-Z][_a-zA-Z0-9]{63,}


    /* comments */
SINGLE_LINE_COMMENT \/\/[^\n]*
MULTI_LINE_COMMENT  \/\*[^*]*\*+([^/*][^*]*\*+)*\/
%%
    /* keywords */
{TYPE}    {
    printf("Matched TOKEN: TYPE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(TYPE);
}
{STRUCT}  {
    printf("Matched TOKEN: STRUCT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(STRUCT);
}
{IF}      {
    printf("Matched TOKEN: IF, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(IF);
}
{ELSE}    {
    printf("Matched TOKEN: ELSE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(ELSE);
}
{WHILE}   {
    printf("Matched TOKEN: WHILE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(WHILE);
}
{RETURN}  {
    printf("Matched TOKEN: RETURN, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(RETURN);
}
    /* INT */
{INT}       {
    printf("Matched TOKEN: INT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(INT);
}
{HEX_INT}   {
    printf("Matched TOKEN: HEX_INT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(INT);
}
{FLOAT}     {
    printf("Matched TOKEN: FLOAT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(FLOAT);
}
{WRONG_HEX_INT} { 
    // fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    printf("Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
    /* single character (printable or hex-form) */
{CHAR}  {
    printf("Matched TOKEN: CHAR, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(CHAR);
}
{WRONG_HEX_CHAR} {
    // fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    printf("Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
{ID}  {
    printf("Matched TOKEN: ID, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(ID);
}
{WRONG_ID} {
    // fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    printf("Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);

    __ASSIGN_LEAF_NODE(ERROR);
}
{DOT}  {
    printf("Matched TOKEN: DOT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(DOT);
}
{SEMI}  {
    printf("Matched TOKEN: SEMI, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(SEMI);
}
{COMMA}  {
    printf("Matched TOKEN: COMMA, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(COMMA);
}
{ASSIGN}  {
    printf("Matched TOKEN: ASSIGN, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(ASSIGN);
}
{LT}  {
    printf("Matched TOKEN: LT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(LT);
}
{LE}  {
    printf("Matched TOKEN: LE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(LE);
}
{GT}  {
    printf("Matched TOKEN: GT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(GT);
}
{GE}  {
    printf("Matched TOKEN: GE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(GE);
}
{NE}  {
    printf("Matched TOKEN: NE, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(NE);
}
{EQ}  {
    printf("Matched TOKEN: EQ, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(EQ);
}
{PLUS}  {
    printf("Matched TOKEN: PLUS, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(PLUS);
}
{MINUS}  {
    printf("Matched TOKEN: MINUS, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(MINUS);
}
{MUL}  {
    printf("Matched TOKEN: MUL, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(MUL);
}
{DIV}  {
    printf("Matched TOKEN: DIV, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(DIV);
}
{AND}  {
    printf("Matched TOKEN: AND, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(AND);
}


{OR}   {
    printf("Matched TOKEN: OR, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(OR);
}
{NOT}  {
    printf("Matched TOKEN: NOT, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(NOT);
}
{LP}  {
    printf("Matched TOKEN: LP, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(LP);
}
{RP}  {
    printf("Matched TOKEN: RP, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(RP);
}
{LB}  {
    printf("Matched TOKEN: LB, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(LB);
}
{RB}  {
    printf("Matched TOKEN: RB, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(RB);
}

{LC}  {
    printf("Matched TOKEN: LC, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(LC);
}
{RC}  {
    printf("Matched TOKEN: RC, Text: %s, Line: %d\n", yytext, yylineno);
    __ASSIGN_LEAF_NODE(RC);
}
    /* comments */
{SINGLE_LINE_COMMENT}   ;
    /* multi-line comments */
"/*"  BEGIN(IN_COMMENT);
    /* whitespace */
\n      ;
[\t ]+ ;
.       {
    // fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    printf("Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        ;
}
%%
