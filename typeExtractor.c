/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: typeExtractor.c

********************/

#include <stdio.h>
#include <string.h>
#include "symbolTable.h"

void get_types(parseTree ast){
	// getting types from declaration stms and formal parameter lists of fns -> done in symbol table construction
	int i;
	for(i=0; i<ast->num_children; i++){
		get_types( ast->children[i]);
	}
	strcpy(ast -> type, "ERROR");//default type if none of the below rules work
	if (!strcmp(symbols[ast->sym], "ID")){
		symbol_table tab = (symbol_table) ast -> sym_table_ptr;
		if(tab==NULL){
			strcpy(ast -> type, "UN"); //undeclared
		} else{
			int ind = find_ht(tab->sym_indices , ast -> t.lxme);
			symbol s = tab -> table[ind];
			strcpy(ast -> type, s.type);
		}
	}
	//primitive types
	if(!strcmp(symbols[ast->sym], "INT") || !strcmp(symbols[ast->sym],"REAL") || !strcmp(symbols[ast->sym],"MATRIX")
			 || !strcmp(symbols[ast->sym],"STRING") ){
		strcpy(ast->type, symbols[ast->sym]);
	}
	//constants
	if(!strcmp(symbols[ast->sym], "NUM"))
		strcpy(ast->type, "INT");
	if(!strcmp(symbols[ast->sym], "RNUM"))
		strcpy(ast->type, "REAL");
	if(!strcmp(symbols[ast->sym], "STR"))
		strcpy(ast->type, "STRING");
	if(!strcmp(symbols[ast->sym], "<matrix>"))
		strcpy(ast->type, "MATRIX");

	//size
	if(!strcmp(symbols[ast->sym], "SIZE")){
		if (!strcmp (ast->children[0]->type,"STRING"))
			strcpy(ast->type,"INT");
		else if (!strcmp (ast->children[0]->type,"MATRIX"))
			strcpy(ast->type,"MAT_SIZE_TYPE");
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - SIZE operation cannot be performed on type %s.\n",
					ast->t.line_no, ast->children[0]->type);
		}
	}

	//operations
	if (!strcmp(symbols[ast->sym], "PLUS")){
		if( !strcmp(ast->children[0]->type, "INT") && !strcmp(ast->children[1]->type, "INT") ){
			strcpy(ast->type, "INT");	
		}
		else if( !strcmp(ast->children[0]->type, "REAL") && !strcmp(ast->children[1]->type, "REAL") ){
			strcpy(ast->type, "REAL");
		}
		else if( !strcmp(ast->children[0]->type, "MATRIX") && !strcmp(ast->children[1]->type, "MATRIX") ){
			strcpy(ast->type, "MATRIX");
		}
		else if( !strcmp(ast->children[0]->type, "STRING") && !strcmp(ast->children[1]->type, "STRING") ){
			strcpy(ast->type, "STRING");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - PLUS operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"MINUS")){
		if( !strcmp(ast->children[0]->type, "INT") && !strcmp(ast->children[1]->type, "INT") ){
			strcpy(ast->type, "INT");	
		}
		else if( !strcmp(ast->children[0]->type, "REAL") && !strcmp(ast->children[1]->type, "REAL") ){
			strcpy(ast->type, "REAL");
		}
		else if( !strcmp(ast->children[0]->type, "MATRIX") && !strcmp(ast->children[1]->type, "MATRIX") ){
			strcpy(ast->type, "MATRIX");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - MINUS operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"MUL")){
		if( !strcmp(ast->children[0]->type, "INT") && !strcmp(ast->children[1]->type, "INT") ){
			strcpy(ast->type, "INT");	
		}
		else if( !strcmp(ast->children[0]->type, "REAL") && !strcmp(ast->children[1]->type, "REAL") ){
			strcpy(ast->type, "REAL");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - MUL operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"DIV")){
		if ( (!strcmp(ast->children[0]->type, "INT") && !strcmp(ast->children[1]->type,"REAL")) 
				||  (!strcmp(ast->children[0]->type,"REAL") && !strcmp(ast->children[1]->type,"REAL") ) ) {
			strcpy( ast -> type, "REAL");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - DIV operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"NOT")){
		if( (!strcmp(ast->children[0]->type,"BOOLEAN")))
			strcpy(ast->type,"BOOLEAN");
		else {
			error_occurred = 1;
			printf("Line #%d: Type mismatch - NOT operation cannot be performed on type %s.\n",
				ast -> t.line_no, ast -> children[0] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"OR") || !strcmp(symbols[ast->sym],"AND") ){
		if( !strcmp(ast->children[0]->type,"BOOLEAN") && !strcmp(ast->children[1]->type,"BOOLEAN") ){
			strcpy(ast->type,"BOOLEAN");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - %s operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, symbols[ast -> sym], ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}
	if(!strcmp(symbols[ast->sym],"LT") ||
		!strcmp(symbols[ast->sym],"LE") ||
		!strcmp(symbols[ast->sym],"GT") ||
		!strcmp(symbols[ast->sym],"GE") ||
		!strcmp(symbols[ast->sym],"EQ") ||
		!strcmp(symbols[ast->sym],"NE") ){
		
		if ( (!strcmp(ast->children[0]->type, "INT") && !strcmp(ast->children[1]->type,"INT")) 
			||  (!strcmp(ast->children[0]->type,"REAL") && !strcmp(ast->children[1]->type,"REAL") ) ) {
			strcpy( ast -> type, "BOOLEAN");
		}
		else{
			error_occurred = 1;
			printf("Line #%d: Type mismatch - Comparison operation cannot be performed on types %s and %s.\n",
					ast -> t.line_no, ast -> children[0] -> type, ast -> children[1] -> type);
		}
	}

	if(!strcmp(symbols[ast->sym],"IF") && strcmp(ast->children[0]->type,"BOOLEAN")){
		error_occurred = 1;
		printf("Line #%d: Type mismatch - conditional part of IF is not of type BOOLEAN.\n",ast->t.line_no);
	}

	if( !strcmp(symbols[ast->sym],"Mat_Elem") ){
		strcpy(ast->type,"INT");
	}

	//Type checking for assignment operations (excluding those where the rhs is a function call)
	if(!strcmp(symbols[ast->sym],"ASSIGNOP") && strcmp(symbols[ast->children[1]->sym],"<funCallStmt>")){
		char * rtype = ast -> children[1] -> type;
		if( strcmp(symbols[ast -> children[0] -> sym], "ID") ){
			//LHS is a list of vars. Only allowed when rhs is size operation for matrix
			if(strcmp(rtype,"MAT_SIZE_TYPE")){
				error_occurred = 1;
				printf("Line #%d: Type mismatch - RHS gives single value, cannot be assigned to list of variables.\n",
						ast -> t.line_no);
			}
		}
		else{
			//get type of variable in LHS (ignore if it has not been declared, that error would have been reported separately)
			symbol_table st = (symbol_table) ast -> children[0] -> sym_table_ptr;
			if(st==NULL)
				return;
			char *ltype = ast -> children[0] -> type; 
			if( strcmp(ltype,rtype) ){
				error_occurred = 1;
				printf("Line #%d: Type mismatch in assignment. LHS is of type %s and RHS is of type %s.\n",
						ast -> t.line_no, ltype, rtype); 
			}
		}
	}

}










