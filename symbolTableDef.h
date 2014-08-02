/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: symbolTableDef.h

********************/

#include "ast.h"

#define NUM_SYM 500

struct sym{
	char id[25];
	char type[25];
	int offset;
	int s1,s2; // size of mat / string
};

struct fun{
	char fun_id[25];
	int in_pars, out_pars, scope;
	void * ast_ptr; 
};


typedef struct sym symbol;
typedef struct fun func_def;
//scopes are enumerated, where 0 is the scope of main

struct sym_table{
	//contains all the symbols & function defs defined in a scope
	char fun_id[25];
	int ifstmt; // 1 if this scope corresponds to an if or else block
	//fun_id if this scope corresponds to a function definition
	int scope;
	int base; //base address of the scope
	int par_scope;
	int num_sym, num_fdef;
	int in_pars, out_pars;
	//only used if the scope is for a function definition
	// first out_pars symbols -> output params of func.
	//next in_pars symbols in the symbol table correspond to input params
	symbol *table; // array of symbols
	Hashtable sym_indices; //maps symbol to index in table

	func_def *fdefs; //array of function defs defined in this scope
	Hashtable fun_indices; // maps fun_id to its index in fdefs
};

typedef struct sym_table * symbol_table;

symbol_table stables[200];
// one symbol table for each scope
