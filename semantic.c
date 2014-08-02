/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: semantic.c

********************/

#include <stdio.h>
#include <string.h>
#include "typeExtractor.h"

int get_str_size(parseTree pt){
	if( !strcmp(symbols[pt -> sym],"ID") ){
		symbol_table tab = (symbol_table) pt -> sym_table_ptr;
		int ind = find_ht(tab -> sym_indices, pt -> t.lxme);
		symbol s = tab -> table[ind];
		return s.s1;
	}
	else if(!strcmp(symbols[pt -> sym],"STR")){
		return strlen(pt->t.lxme) - 2; //quotes not to be counted
	}
	else if(!strcmp(symbols[pt -> sym],"PLUS")){
		return get_str_size(pt->children[0]) + get_str_size(pt->children[1]);
	}
	
}

void get_mat_size(parseTree mat_ast, int *r, int *c){
	if( !strcmp(symbols[mat_ast -> sym],"ID") ){
		symbol_table tab = (symbol_table) mat_ast -> sym_table_ptr;
		int ind = find_ht(tab -> sym_indices, mat_ast -> t.lxme);
		symbol s = tab -> table[ind];
		*r = s.s1;
		*c = s.s2;
		return;
	}
	else if(!strcmp(symbols[mat_ast -> sym],"<matrix>")){
		*r = mat_ast -> num_children;
		*c = mat_ast -> children[0] -> num_children;
		int i;
		for(i=1;i<(*r);i++){
			if(mat_ast -> children[i] -> num_children != (*c) ){
				*c = -1; // two cols have different sizes
				break;
			}
		}
	}
	else if(!strcmp(symbols[mat_ast -> sym],"PLUS") || !strcmp(symbols[mat_ast -> sym],"MINUS")  ){
		int r1 =-1, r2 =-1, c1 =-1, c2 =-1;
		get_mat_size(mat_ast->children[0],&r1,&c1);
		get_mat_size(mat_ast->children[1],&r2,&c2);
		if(r1==r2 && c1==c2){
				*r = r1, *c = c1;
		}
		else *r = -1, *c = -1;
	}
}

void semantic_check(parseTree ast){

	int i;
	for(i=0;i<ast->num_children;i++){
		semantic_check(ast->children[i]);
	}

	if( !strcmp( symbols[ast->sym] , "<funCallStmt>") ){
	//semantics of function call
		symbol_table tab = (symbol_table) ast -> children[0] -> sym_table_ptr;
		if(tab == NULL){
			return;
		}
		func_def fd = tab -> fdefs[find_ht(tab -> fun_indices, ast->children[0]->t.lxme)];
		//function with no return value should be appropriately called
		if( fd.in_pars != ast -> num_children - 1){
			error_occurred = 1;
			printf("Line #%d: Function %s needs %d inputs; %d inputs have been passed.\n",
					ast -> children[0] -> t.line_no, ast -> children[0] -> t.lxme, fd.in_pars, ast ->  num_children - 1);
		}
		else if( fd.out_pars == 0){
			//function call's parent should not be an assignemnt operation
			if(! strcmp ("ASSIGNOP", symbols[ast -> parent -> sym] ) ){
				error_occurred = 1;
				printf("Line #%d: Function %s with no return values can not be a part of assignment statement\n",
						ast -> parent -> t.line_no, ast -> children[0] -> t.lxme);
			}
		}
		else if(strcmp(symbols[ast->parent->sym],"ASSIGNOP")){
			error_occurred = 1;
			printf("Line #%d: Return value(s) of function %s should be assigned to appropriate variables.\n",
					ast -> children[0] -> t.line_no, ast -> children[0] -> t.lxme );
		}
		else{
			if( (fd.out_pars==1 && strcmp(symbols[ast -> parent -> children[0] -> sym],"ID") )
						|| (fd.out_pars>1 && (!strcmp(symbols[ast -> parent -> children[0] -> sym],"ID")  
								|| (ast -> parent -> children[0] -> num_children != fd.out_pars) ) ) ){
				error_occurred = 1;
				printf("Line #%d: Number of variables on LHS does not match number of variables returned by %s.\n",
						ast -> children[0] -> t.line_no, ast -> children[0] -> t.lxme) ;
			}
			symbol_table tb = stables[fd.scope];
			int j;
			for(j=0;j<fd.in_pars;j++){
				char *t_req = tb -> table[fd.out_pars + j] . type;
				symbol_table stb = (symbol_table) ast -> children[j+1] -> sym_table_ptr;
				if(stb==NULL){
					return;
				}
				char *t_prov = stb -> table[find_ht(stb -> sym_indices, ast -> children[j+1] -> t.lxme)] . type;
				if(strcmp(t_req, t_prov)){
					error_occurred = 1;
					printf("Line #%d: input parameter no. %d of function %s is of type %s; passed parameter is of type %s.\n",
							ast -> children[0] -> t.line_no, j+1, ast -> children[0] -> t.lxme, t_req, t_prov);
				}
			}
			if(fd.out_pars==1){
				symbol_table stb = (symbol_table) ast -> parent -> children[0] -> sym_table_ptr;
				if(stb==NULL)
					return;
				char *t_prov = stb -> table[ find_ht(stb -> sym_indices, ast -> parent -> children[0] -> t.lxme) ] . type;
				char *t_req  = tb -> table[0] . type;
				if(strcmp(t_req, t_prov)){
					error_occurred = 1;
					printf("Line #%d: function %s returns value of type %s, but return value has been assigned to type %s.\n",
							ast -> children[0] -> t.line_no, ast -> children[0] -> t.lxme, t_req, t_prov);
				}
			}
			else{
				for(j=0;j<fd.out_pars;j++){
					char *t_req = tb -> table[j] . type;
					symbol_table stb = (symbol_table) (ast -> parent -> children[0] -> children[j] -> sym_table_ptr);
					if(stb==NULL)
						return;
					char * t_prov = stb -> table[find_ht(stb->sym_indices, ast -> parent -> children[0] -> children[j] -> t.lxme)] . type;
					if(strcmp(t_req, t_prov)){
						error_occurred = 1;
						printf("Line #%d: output parameter no. %d of function %s is of type %s; has been assigned to type %s.\n",
								ast -> children[0] -> t.line_no, j+1, ast -> children[0] -> t.lxme, t_req, t_prov);
					}

				}
			}
		}
	}
	if ( !strcmp (symbols[ast->sym], "<matrix>") ){
		int num_rows = ast -> num_children; // each child corresponds to a row.
		// check if cols are equal in each row
		int i, num_cols = ast -> children[0] -> num_children;
		int correct = 1;
		for(i=1;i<num_rows;i++){
			if( ast -> children[i] -> num_children != num_cols){
				error_occurred = 1;
				printf("Line #%d: Matrix incorrectly assigned. All rows do not have same no. of columns\n",ast->children[0]->children[0]->t.line_no);
				correct = 0;
				break;
			}
		}
	}
	//adding or subtracting two matrices -> check if same size
	if( !strcmp (symbols[ast->sym], "PLUS") || !strcmp (symbols[ast->sym],"MINUS") ){
		if (!strcmp("MATRIX", ast -> children[0] -> type) && !strcmp("MATRIX", ast -> children[1] -> type) ){
			int r1,r2,c1,c2;
			get_mat_size(ast -> children[0], &r1, &c1);
			get_mat_size(ast -> children[1], &r2, &c2);
			if(r1!=r2 || c1!=c2){
				error_occurred = 1;
				printf("Line #%d: Cannot %s two matrices with different sizes.\n", ast -> t. line_no, 
						strcmp(symbols[ast->sym],"PLUS") ? "subtract" : "add" );
			}
		}
	}

	if(!strcmp(symbols[ast->sym],"ASSIGNOP") && !strcmp(symbols[ast->children[0]->sym],"ID")){
		//check for string or matrix.
		symbol_table st = ast -> children[0] -> sym_table_ptr;
		symbol s = st -> table [find_ht( st -> sym_indices, ast -> children[0] -> t.lxme)];
		if(!strcmp( symbols[ast -> children[1] -> sym], "MATRIX" )){
			get_mat_size(ast -> children[1], &s.s1, &s.s2);
		}
		else if(!strcmp (symbols[ast->children[1] -> sym], "STRING")){
			s.s1 = get_str_size(ast -> children[1]);
		}
		//matrix element indexing - check that it is not out of bounds
		else if(!strcmp( symbols[ast->children[1] -> sym], "Matrix_Elem") ){
			int r = ast->children[1]->children[1] -> t.i_val,  c = ast -> children[1] -> children[2] -> t.i_val;
			if(r >= s.s1 || c >= s.s2){ //assuming 0-indexing
				error_occurred = 1;
				printf("Line #%d: Matrix index out of bounds.\n",
						ast -> children[0] -> children[0] -> t.line_no);
			}
		}
	}
}









