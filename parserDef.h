/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: parserDef.h

********************/


#include "linked_list.h"
#include "lexer.h"

#define NUM_SYMBOLS 200
#define NUM_RULES 400

struct rule{
	int lhs;
	linked_list rhs;
};

int num_rules;

typedef struct rule grammar_rule;
typedef grammar_rule ast_rule;
typedef grammar_rule *grammar;
typedef grammar sets;
typedef grammar ast_rules;

DFA d;
grammar g;

char symbols[NUM_SYMBOLS][25];
int num_terminals; //read_dfa finds this number
int num_symbols, start_symbol;
Hashtable symbol_nos; //maps symbols to their enumerated nos
int num_non_terminals;

struct tnode;
typedef struct tnode Tnode;
typedef Tnode *parseTree;

struct tnode{
	int sym;
	int num_children;
	Tnode *parent;
	parseTree children[30];
	int is_leaf,is_root;
	int children_seen;
	int gr_rule; //grammar rule no. used to expand if non-terminal
	tokenInfo t; // if leaf
	int discard;
	void * sym_table_ptr;
	char type[30];
};

typedef int table[NUM_SYMBOLS][NUM_SYMBOLS];
