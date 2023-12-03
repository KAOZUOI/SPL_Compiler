%{
    #include "APT.h"
    #include "lex.yy.c"
    #include "semantic.h"

    FILE* fout = NULL;

    #define MISSING_SEMI_ERROR(e) { \
        ++errors; \
        printf("Error type B at Line %d: Missing semicolon ';'\n", e->lineno); \
    }
    // fprintf(fout, "Error type B at Line %d: Missing semicolon ';'\n", e->lineno);

    #define MISSING_RP_ERROR(e) { \
        ++errors; \
        printf("Error type B at Line %d: Missing closing parenthesis ')'\n", e->lineno); \
    }
    // fprintf(fout, "Error type B at Line %d: Missing closing parenthesis ')'\n", e->lineno);  
    #define MISSING_SEMI_ERROR_LOC1(e) { \
        ++errors; \
        printf("Error type B at Line %d: Missing semicolon ';' LOC1\n", e->lineno); \
    }
    #define MISSING_SEMI_ERROR_LOC2(e) { \
        ++errors; \
        printf("Error type B at Line %d: Missing semicolon ';' LOC2\n", e->lineno); \
    }
    #define MISSING_SEMI_ERROR_LOC3(e) { \
        ++errors; \
        printf("Error type B at Line %d: Missing semicolon ';' LOC3\n", e->lineno); \
    }

    uint32_t errors = 0;

    void yyerror (const char*);
%}

%union {
    AnnotatedParseTreeNode* AptNode;
}

%token <AptNode> INT     // /* integer in 32-bits (decimal or hexadecimal) */
%token <AptNode> HEX_INT
%token <AptNode> FLOAT   // /* floating point number (only dot-form) */
%token <AptNode> CHAR    // /* single character (printable or hex-form) */
%token <AptNode> ID      // /* identifier */
%token <AptNode> TYPE    // int | float | char
%token <AptNode> STRUCT  // struct
%token <AptNode> IF      // if
%token <AptNode> ELSE    // else
%token <AptNode> WHILE   // while
%token <AptNode> RETURN  // return
%token <AptNode> DOT     // .
%token <AptNode> SEMI    // ;
%token <AptNode> COMMA   // ,
%token <AptNode> ASSIGN  // =
%token <AptNode> LT      // <
%token <AptNode> LE      // <=
%token <AptNode> GT      // >
%token <AptNode> GE      // >=
%token <AptNode> NE      // !=
%token <AptNode> EQ      // ==
%token <AptNode> PLUS    // +
%token <AptNode> MINUS   // -
%token <AptNode> MUL     // *
%token <AptNode> DIV     // /
%token <AptNode> AND     // &&
%token <AptNode> OR      // ||
%token <AptNode> NOT     // !
%token <AptNode> LP      // (
%token <AptNode> RP      // )
%token <AptNode> LB      // [
%token <AptNode> RB      // ]
%token <AptNode> LC      // {
%token <AptNode> RC      // }
%token <AptNode> ERROR   // Lexical error
//%token <AptNode> NEWLINE

%type <AptNode> Program ExtDefList ExtDef ExtDecList;
%type <AptNode> Specifier StructSpecifier;
%type <AptNode> VarDec FunDec VarList ParamDec;
%type <AptNode> CompSt StmtList Stmt;
%type <AptNode> DefList Def DecList Dec;
%type <AptNode> Exp Args;

%right ASSIGN
%right NOT

%left OR
%left AND
%left LT LE GT GE NE EQ
%left PLUS MINUS
%left MUL DIV
%left COMMA DOT

%nonassoc LP RP LB RB LC RC
%nonassoc SEMI
%nonassoc ELSE
//注意Def与Dec的区别
%%
    /* high-level definition */
Program: 
        ExtDefList    { 
                $$ = newAnnotatedParseNode("Program", 1, $1);
                //if (errors == 0)    printAnnotatedParseTree($$, 0);
                programSemaParser($$);
            }
    ;
ExtDefList: 
        ExtDef ExtDefList    { $$ = newAnnotatedParseNode("ExtDefList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
//每一个 ExtDef 表示全局变量，结构体或函数定义
ExtDef:
        Specifier ExtDecList SEMI    { $$ = newAnnotatedParseNode("ExtDef", 3, $1, $2, $3); }
    |   Specifier SEMI    { $$ = newAnnotatedParseNode("ExtDef", 2, $1, $2); }
    |   Specifier FunDec CompSt    { $$ = newAnnotatedParseNode("ExtDef", 3, $1, $2, $3); }
    |   Specifier ExtDecList error {
        $$ = newAnnotatedParseNode("ExtDef", 2, $1, $2);
        // MISSING_SEMI_ERROR($1);
        MISSING_SEMI_ERROR_LOC1($1);//全局变量定义语句缺少;
    }
    |   Specifier FunDec error {
        $$ = newAnnotatedParseNode("ExtDef", 1, $1);
        // MISSING_SEMI_ERROR($1);
        MISSING_SEMI_ERROR_LOC2($1);//函数声明语句缺少;
    }
    |   Specifier error {
        $$ = newAnnotatedParseNode("ExtDef", 1, $1);
        // MISSING_SEMI_ERROR($1);
        MISSING_SEMI_ERROR_LOC3($1);//结构体定义语句缺少;
    }
    |   error SEMI {
        ++errors;
    }
    ;
//表示零个或多个 对一个变量的定义VarDec, 例如 int a, b, c;
ExtDecList:
        VarDec    { $$ = newAnnotatedParseNode("ExtDecList", 1, $1); }
    |   VarDec COMMA ExtDecList    { $$ = newAnnotatedParseNode("ExtDecList", 3, $1, $2, $3); }
    ;

    /* specifier */
//表示一个变量的类型或者一个结构体的定义
Specifier:
        TYPE    { $$ = newAnnotatedParseNode("Specifier", 1, $1); }
    |   StructSpecifier    { $$ = newAnnotatedParseNode("Specifier", 1, $1); }
    ;
//定义结构体的基本格式 struct id { ... }
//如果之前已经定义过某个结构体，比如struct Complex {…}，那么之后可以直接使用该结构体来定义变量
StructSpecifier:
        STRUCT ID LC DefList RC    { $$ = newAnnotatedParseNode("StructSpecifier", 5, $1, $2, $3, $4, $5); }
    |   STRUCT ID    { $$ = newAnnotatedParseNode("StructSpecifier", 2, $1, $2); }
    ;

    /* declarator */
//表示一个变量的定义，其中包含变量名或 变量名与数组大小 a[2][3]
VarDec: 
        ID    { $$ = newAnnotatedParseNode("VarDec", 1, $1); }
    // |   VarDec ASSIGN Exp    { $$ = newAnnotatedParseNode("VarDec", 3, $1, $2, $3); }
    |   ERROR {
        ++errors;
        $$ = newAnnotatedParseNode("VarDec", 1, $1);
    }
    |   VarDec LB INT RB    { $$ = newAnnotatedParseNode("VarDec", 4, $1, $2, $3, $4); }
    |   VarDec LB HEX_INT RB    { $$ = newAnnotatedParseNode("VarDec", 4, $1, $2, $3, $4); }
    ;
//表示对一个函数头的定义，其中包含函数名和参数列表，例如 int f(int a, int b)
FunDec:
        ID LP VarList RP    { $$ = newAnnotatedParseNode("FunDec", 4, $1, $2, $3, $4); }
    |   ID LP RP    { $$ = newAnnotatedParseNode("FunDec", 3, $1, $2, $3); }
    |   ID LP VarList error    {
        $$ = newAnnotatedParseNode("FunDec", 3, $1, $2, $3);
        MISSING_RP_ERROR($3);
    }
    |   ID LP error    {
        $$ = newAnnotatedParseNode("FunDec", 2, $1, $2);
        MISSING_RP_ERROR($2);
    }
    ;
//包括一个或多个ParamDec，其中每个ParamDec都是对一个形参的定义
VarList:
        ParamDec    { $$ = newAnnotatedParseNode("VarList", 1, $1); }
    |   ParamDec COMMA VarList    { $$ = newAnnotatedParseNode("VarList", 3, $1, $2, $3); }

    ;
ParamDec:
        Specifier VarDec    { $$ = newAnnotatedParseNode("ParamDec", 2, $1, $2); }
    ;

    /* statement */
//表示一个由一对花括号括起来的语句块。
//该语句块内部先是一系列的变量定义DefList，然后是一系列的语句StmtList。
//对CompSt这样的定义，是不允许在程序的任意位置定义变量的，必须在每一个语句块的开头才可以定义。
CompSt: 
        LC DefList StmtList RC    { $$ = newAnnotatedParseNode("CompSt", 4, $1, $2, $3, $4); }
    ;
StmtList: 
        Stmt StmtList    { $$ = newAnnotatedParseNode("StmtList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
//每个Stmt都表示一条语句，包括赋值语句、条件语句、循环语句、返回语句、语句块等等。
Stmt:
       Exp SEMI    { $$ = newAnnotatedParseNode("Stmt", 2, $1, $2); }
    |   CompSt    { $$ = newAnnotatedParseNode("Stmt", 1, $1); }
    |   RETURN Exp SEMI    { $$ = newAnnotatedParseNode("Stmt", 3, $1, $2, $3); }
    |   IF LP Exp RP Stmt    { $$ = newAnnotatedParseNode("Stmt", 5, $1, $2, $3, $4, $5); }
    |   IF LP Exp RP Stmt ELSE Stmt    { $$ = newAnnotatedParseNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    |   WHILE LP Exp RP Stmt    { $$ = newAnnotatedParseNode("Stmt", 5, $1, $2, $3, $4, $5); }
    |   Exp error   { 
        $$ = newAnnotatedParseNode("Stmt", 1, $1);
        MISSING_SEMI_ERROR_LOC2($1); //表达式语句缺少;
    }
    |   RETURN Exp error    {
        $$ = newAnnotatedParseNode("Stmt", 2, $1, $2);
        MISSING_SEMI_ERROR_LOC1($2); //返回语句缺少;
    }
    |   IF LP Exp error Stmt    { 
        $$ = newAnnotatedParseNode("Stmt", 4, $1, $2, $3, $5);
        MISSING_RP_ERROR($3); //if语句缺少)
    }
    |   WHILE LP Exp error Stmt    {
        $$ = newAnnotatedParseNode("Stmt", 3, $1, $2, $3, $5);
        MISSING_RP_ERROR($3); //while语句缺少)
    }
    ;

    /* local definition */
// 它就是一串像int a; float b, c; int d[10];这样的变量定义。
// 一个DefList可以由零个或者多个Def组成。
//允许我们对局部变量在定义时进行初始化，例如int a = 5;
DefList: 
        Def DefList   { $$ = newAnnotatedParseNode("DefList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
// 一条变量定义，例如int a, b, c;
Def:
        Specifier DecList SEMI    { $$ = newAnnotatedParseNode("Def", 3, $1, $2, $3); }
    |   Specifier DecList error    {
        $$ = newAnnotatedParseNode("Def", 2, $1, $2);
        MISSING_SEMI_ERROR($1); //局部变量定义语句缺少;
    }
    ;
// 一串变量的定义，例如a, b, c
DecList: 
        Dec    { $$ = newAnnotatedParseNode("DecList", 1, $1); }
    |   Dec COMMA DecList    { $$ = newAnnotatedParseNode("DecList", 3, $1, $2, $3); }
    ;

// VarDec 表示一个变量的定义，例如int a[10][20]，其中包含变量名或 变量名与数组大小 a[2][3]
//允许我们对局部变量在定义时进行初始化，例如int a = 5;
Dec: 
        VarDec    { $$ = newAnnotatedParseNode("Dec", 1, $1); }
    |   VarDec ASSIGN Exp    { $$ = newAnnotatedParseNode("Dec", 3, $1, $2, $3); }
    ;
    /* Expression */
//表示一个表达式，包括一元运算、二元运算、函数调用、数组访问、结构体访问、变量访问、常量等等。
Exp:  
        Exp ASSIGN Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp AND Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp OR Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LT Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp GT Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp GE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp NE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp EQ Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp PLUS Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp MINUS Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp MUL Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp DIV Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   LP Exp RP    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   MINUS Exp    { $$ = newAnnotatedParseNode("Exp", 2, $1, $2); }
    |   NOT Exp    { $$ = newAnnotatedParseNode("Exp", 2, $1, $2); }
    |   ID LP Args RP    { $$ = newAnnotatedParseNode("Exp", 4, $1, $2, $3, $4); }
    |   ID LP RP    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LB Exp RB    { $$ = newAnnotatedParseNode("Exp", 4, $1, $2, $3, $4); }
    |   Exp DOT ID    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   ID    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   INT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   HEX_INT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   FLOAT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   CHAR    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   Exp ERROR Exp   {
        ++errors;
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3);
    }
    |   ERROR   {
        ++errors;
        $$ = newAnnotatedParseNode("Exp", 1, $1);
    }
    |   LP Exp error    {
        $$ = newAnnotatedParseNode("Exp", 2, $1, $2);
        MISSING_RP_ERROR($2);
    }
    |   ID LP Args error    {
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); 
        MISSING_RP_ERROR($3);
    }
    |   ID LP error    {
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2);
        MISSING_RP_ERROR($2);
    }
    ;
Args: Exp COMMA Args    { $$ = newAnnotatedParseNode("Args", 3, $1, $2, $3); }
    |   Exp    { $$ = newAnnotatedParseNode("Args", 1, $1); }
    ;
%%
void yyerror (char const *s) {

//   fprintf (stderr, "ERROR: Line %d, %s \"%s\"\n", yylineno, s, yytext);
    //  printf("ERROR: Line %d, %s \"%s\"\n", yylineno, s, yytext);

}

int main (int argc, char **argv) {
    char* tmp = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 3));
    strcpy(tmp, argv[1]);
    char *end = tmp + strlen(tmp);
    while (end > tmp && *end != '.') {
        --end;
    }
    if (end > tmp) {
        *(end + 1)= 'o';
        *(end + 2)= 'u';
        *(end + 3)= 't';
    }
    fout = fopen(tmp, "w");
    printf("%s\n", tmp);
    yyin = fopen(argv[1], "r");
    yyparse();
    /*while(1) {
        int token = yylex();
        if (token == 0) break;
        printf("Token: %d, value: '%s'\n", token, yytext);
    }*/
}