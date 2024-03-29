#ifndef APT_H
#define APT_H





extern char* yytext;

typedef struct AnnotatedParseTreeNode {
    int lineno;
    char* name;

    struct AnnotatedParseTreeNode* left;
    struct AnnotatedParseTreeNode* right;

    // Node value
    union
    {
        int int_value;
        float float_value;
        char* string_value;
    };

    short is_token;
    
} AnnotatedParseTreeNode;
typedef AnnotatedParseTreeNode* Node;

AnnotatedParseTreeNode* newAnnotatedParseNode(char* name, int cnt, ...);

AnnotatedParseTreeNode* newAnnotatedParseLeafNode(char* name, int lineno);

void printAnnotatedParseTree(AnnotatedParseTreeNode* apt, int indent);

#endif // APT_H