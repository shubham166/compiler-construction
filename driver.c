/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: driver.c

********************/

#include <stdio.h>
#include <stdlib.h>
#include "code_gen.h"

int main(int argc, char *args[]){
	table T;
	read_grammar();
	createParseTable(g,T);
	int option;
	if(argc < 3){
		printf("You have not provided the required number of command-line arguments\n");
		exit(0);
	}
	char *test_file = args[1];
	char *output_file = args[2];
	printf("Level 3: All modules work.\n");
	printf("**********************************************************************\n");
	printf("Please enter one of the following options:\n");
	printf("1. Print the list of tokens.\n");
	printf("2. Verify the syntactic correctness of the code and generate parse tree\n");
	printf("3. Create and print the abstract syntax tree.\n");
	printf("4. Create and print symbol table.\n");
	printf("5. Print the semantic errors in the code.\n");
	printf("6. Generate the assembly code.\n");
	scanf("%d",&option);
	if(option==1){
		open_file(test_file);
		printf("%15s | %25s |L_NO\n","TOKEN","LEXEME");
		printf("-----------------------------------------------------------\n");
		tokenInfo t;
		do{
			t = getNextToken(&d);
			printf("%15s | %25s | %d\n",t.token,t.lxme,t.line_no);
		}while(strcmp(t.token,"$"));
		close_file();
	}
	else if(option==2){
		parseTree pt = parseInputSourceCode(test_file,T);
		printParseTree(pt);
		printf("There are no syntax errors. Parse tree:\n");
	}
	else if(option==3){
		parseTree PT = parseInputSourceCode(test_file,T);
		read_ast_rules();
		create_ast(PT);
		create_symbol_table(PT,0,0);
		printParseTree(PT);
	}
	else if(option==4){
		parseTree PT = parseInputSourceCode(test_file,T);
		read_ast_rules();
		create_ast(PT);
		create_symbol_table(PT,0,0);
		print_symbol_table();	
	}
	else if(option==5){
		parseTree PT = parseInputSourceCode(test_file,T);
		read_ast_rules();
		create_ast(PT);
		create_symbol_table(PT,0,0);
		get_types(PT);
		semantic_check(PT);
	}
	else if(option==6){
		parseTree PT = parseInputSourceCode(test_file,T);
		read_ast_rules();
		create_ast(PT);
		create_symbol_table(PT,0,0);
		get_types(PT);
		semantic_check(PT);
		if(!error_occurred){
			code_generation(PT,output_file);
			printf("Code compiled successfully... \n");
		}
	}
	return 0;
}



