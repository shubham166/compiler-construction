/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: symbolTable.c

********************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "symbolTableDef.h"
int error_occurred = 0;
symbol_table new_stable(int scope_no, int par_no){
	symbol_table st = (symbol_table) malloc(sizeof(struct sym_table));
	st -> table = (symbol *) malloc(NUM_SYM*sizeof(symbol));
	st -> sym_indices = new_HT(NUM_SYM);
	st -> fdefs = (func_def *) malloc(NUM_SYM*sizeof(func_def));
	st -> fun_indices = new_HT(NUM_SYM);
	st -> scope = scope_no;
	st -> par_scope = par_no;
	st -> num_sym = st -> num_fdef = 0;
	st -> ifstmt = 0;
	if(scope_no==0)
		strcpy(st -> fun_id, "MAIN");
	else
		strcpy(st -> fun_id, stables[par_no] -> fun_id);
	return st;
}

int get_size(char *type){
	if( !strcmp(type,"INT") )
		return 2;
	if( !strcmp(type,"REAL"))
		return 4;
	if( !strcmp(type,"STRING"))
		return 21;  //one extra for demarcating character used by us 
	if( !strcmp(type,"MATRIX"))
		return 100; // assuming 10 x 10
}

int num_scopes = 0;
void create_symbol_table(parseTree ast, int scope, int parent_scope){
	int ch_scope = scope, i;
	if( !strcmp(symbols[ast->sym], "MAIN") ){
		num_scopes = 1;
		stables[0] = new_stable(0,0);
		stables[0] -> base = 100; // change if needed
		strcpy(stables[0] -> fun_id , "MAIN");
	}
	else if( !strcmp(symbols[ast->sym], "IF") ){
		//new scope for if block
		ch_scope = num_scopes++;
		int ch_base;
		symbol_table tab = stables[scope];
		if(tab -> num_sym == 0)
			ch_base = 0;
		else ch_base = 
					tab -> table[tab -> num_sym - 1].offset 
							+ get_size(tab -> table[tab -> num_sym] . type) ;
		stables[ch_scope] = new_stable(ch_scope, scope);
		stables[ch_scope] -> base = ch_base;
		stables[ch_scope] -> ifstmt = 1;
		for(i=0; i < ast->num_children ; i++){
			create_symbol_table( ast->children[i], ch_scope, scope);
			if( !strcmp( symbols[ast->children[i]->sym] , "ELSE" ) ){
				//new scope for else block
				ch_scope = num_scopes++;
				stables[ch_scope] = new_stable(ch_scope, scope);
				stables[ch_scope] -> ifstmt = 1;
				stables[ch_scope] -> base = ch_base;
				//base will be same as that of if block since we are out of if block
			}
		}
		return;
	}
	else if( !strcmp(symbols[ast->sym], "<functionDef>") ){
		ch_scope = num_scopes;
		symbol_table tab = stables[scope], t1 = tab;
		while(t1->scope!=0 && find_ht(t1 -> fun_indices, ast -> children[1] -> t.lxme)==-1){
			t1 = stables[t1 -> par_scope]; //keep checking parent if not found
		}
		if( find_ht(t1 -> fun_indices, ast -> children[1] -> t.lxme) !=-1){
			//already declared
			error_occurred = 1;
			printf("Line #%d: Overloading of function %s.\n",
					ast -> children[1] -> t. line_no, ast -> children[1] -> t.lxme);
		}
		int f_no = tab -> num_fdef;
		tab -> num_fdef += 1;
		strcpy( (tab->fdefs[f_no]).fun_id, (ast -> children[1]-> t).lxme );
		stables[num_scopes++] = new_stable(ch_scope, scope);
		insert_ht(tab -> fun_indices,  ast -> children[1]-> t.lxme, f_no);
		ast -> sym_table_ptr = tab;
		(tab -> fdefs[f_no]).out_pars = stables[ch_scope] -> out_pars = (ast -> children[0] -> num_children)/2;
		(tab -> fdefs[f_no]).in_pars = stables[ch_scope] -> in_pars	= (ast -> children[2] -> num_children)/2;
		strcpy( stables[ch_scope] -> fun_id, (ast->children[1] -> t).lxme);
		(tab -> fdefs[f_no]).scope = ch_scope;
		strcpy( stables[ch_scope] -> fun_id , (ast -> children[1] -> t).lxme );
		(tab -> fdefs[f_no]).ast_ptr = ast;
	}
	else if( !strcmp (symbols[ast->sym], "<parameter_list>") ){
		// alternate children are types and ids (eg. int a, string b, real c)
		symbol_table tab = stables[scope];
		for(i=1; i < ast -> num_children; i+=2){
			strcpy((tab -> table[tab->num_sym]).type, symbols[ast -> children[i-1] ->sym]);
			strcpy((tab -> table[tab->num_sym]).id, (ast->children[i]->t).lxme);
			insert_ht(tab -> sym_indices, tab -> table[tab->num_sym].id, tab->num_sym);
			if(tab -> num_sym == 0){
				tab -> table[tab -> num_sym].offset = 0; //first symbol in the scope
			} else
				tab -> table[tab -> num_sym].offset = 
					tab -> table[tab -> num_sym - 1].offset 
							+ get_size(tab -> table[tab -> num_sym] . type) ;
			ast -> children[i] -> sym_table_ptr = tab;
			tab -> num_sym += 1;
		}
		return;
	}
	else if( !strcmp(symbols[ast->sym], "<declarationStmt>") ){
		//child[0] is the type, others are ids
		for( i=1; i < ast->num_children; i++){
			symbol_table tab = stables[scope], t1 = tab;;
			while (t1 -> scope !=0 && find_ht(t1->sym_indices, (ast->children[i]->t).lxme) ==-1){
				t1 = stables[t1->par_scope];
			}
			if( find_ht(t1 -> sym_indices, (ast -> children[i] -> t).lxme) !=-1){
				error_occurred = 1;
				printf("Line #%d: Redeclaration of variable %s\n",ast->children[i]->t.line_no,
						ast->children[i]->t.lxme);
			} else{
				strcpy( (tab -> table[tab -> num_sym]).id, (ast->children[i]->t).lxme);
				strcpy( (tab -> table[tab -> num_sym]).type, symbols[ast->children[0]->sym]);
				ast -> children[i] -> sym_table_ptr = tab;
				insert_ht(tab -> sym_indices, tab -> table[tab->num_sym].id, tab->num_sym);
				if(tab -> num_sym == 0){
					tab -> table[tab -> num_sym].offset = 0; //first symbol in the scope
				} else
					tab -> table[tab -> num_sym].offset = 
						tab -> table[tab -> num_sym - 1].offset 
								+ get_size(tab -> table[tab -> num_sym - 1] . type) ;
				tab -> num_sym += 1;
			}
		}
		return;
	}
	else if( !strcmp(symbols[ast->sym], "ID") ){
		symbol_table tab = stables[scope];
		while (tab -> scope !=0 && find_ht(tab->sym_indices, (ast->t).lxme) ==-1){
			tab = stables[tab->par_scope];
		}
		if( find_ht( tab -> sym_indices, (ast -> t).lxme) != -1){
			ast -> sym_table_ptr = tab;
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Symbol %s has not been declared\n",
					ast -> t.line_no, ast -> t.lxme);
			ast -> sym_table_ptr = NULL;
		}
		return;
	}
	else if( !strcmp( symbols[ast->sym],"FUNID") ){
		symbol_table tab = stables[scope];
		if(!strcmp(symbols[ast->parent->sym],"<funCallStmt>")){
			//check recursion
			if(!strcmp(ast -> t.lxme, tab -> fun_id)){
				error_occurred = 1;
				printf("Line #%d: Function %s is recursively called.\n",
						ast -> t.line_no, ast -> t.lxme);
			}
			//function must be declared outside calling function (semantic rule 14)
			symbol_table t1 = tab;
			while(t1 -> ifstmt){
				t1 = stables[t1->par_scope];
			}
			//t1 is now the scope of the calling function
			char *calling_fn = t1 -> fun_id;
			t1 = stables[t1 -> par_scope];
			while( t1 -> scope !=0 && find_ht(t1 -> fun_indices, ast -> t.lxme)==-1 )
				t1 = stables[t1 -> par_scope];
			if( find_ht(t1 -> fun_indices, ast -> t.lxme) == -1){
				//not declared
				error_occurred = 1;
				printf("Line #%d: Function %s ",ast->t.line_no , ast->t.lxme);
				if(!strcmp(calling_fn,"MAIN"))
					printf("has not been declared.\n");
				else printf("has not been declared before its calling function %s\n",calling_fn);
			}
			else{
				ast -> sym_table_ptr = t1;
			}
		}
	}
	for(i=0; i < ast->num_children ; i++){
		create_symbol_table( ast->children[i], ch_scope, scope);
	}
}

void print_symbol_table(){
	int i;
	printf("%35s | %50s | %25s | offset\n","Symbol","Scope","Type","Offset");
	printf("----------------------------------------------------------------------------------------------------------------------------------------------\n");
	for(i=0;i<num_scopes;i++)
	{
		symbol_table tab = stables[i];
		int j;
		for(j=0;j< tab->num_sym ;j++){
			symbol s = tab -> table[j];
			char scp[50];
			if(tab -> ifstmt)
				sprintf(scp,"if/else stmt in %s",tab -> fun_id);
			else strcpy(scp,tab ->fun_id);
			printf("%35s | %50s | %25s | %5d\n",s.id, scp, s.type, s.offset);
		}
	}
}

