#include "parser.h"

struct ast_node;

typedef struct ast_node ANode;
typedef ANode * AST;

struct ast_node{
	int sym;
	AST parent;
	AST children[20];
	int num_ch;
	int is_leaf, is_root;
	tokenInfo t;
	//TODO: add symbol table 
	// pointer
};
