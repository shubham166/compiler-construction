#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "semantic.h"

int if_id = 0;
FILE *fp;
void write_header()
{
	char s[100];

	fprintf(fp, ".MODEL SMALL\n");
	fprintf(fp, ".STACK 100\n");
	fprintf(fp, ".DATA\n");
	fprintf(fp, "MAX1 DB 20\n");
	fprintf(fp, "ACT1 DB ?\n");
	fprintf(fp, "INP1 DB 32 DUP(0)\n");
	fprintf(fp, ".CODE\n");
	fprintf(fp, ".STARTUP\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	// fprintf(fp,s);
}

void wrtie_footer()
{
	fprintf(fp, ".EXIT\n");
	fprintf(fp, "END\n" );
}
/*void cgen_varallocate(table t,variable v)
{
	int base_address,offset;
	//TODO : get base adress and offset of variable v;
	fprintf(fp, "mov SI ,%d\n",base_address+offset);
	fprintf(fp, "mov [SI],\n", );
}
*/
void cgen_io(parseTree pt,char *cmd);
void cgen_getvar(parseTree pt);
void cgen_assignment(parseTree pt1 , parseTree pt2);
void cgen_arithmetic_string(parseTree pt1,parseTree pt2);
void cgen_arithmetic(parseTree pt1, char *op, parseTree pt2);
void cgen_ifstmt(parseTree pt,int true_stmts, int is_else, int false_stmts);
void cgen_ifstmt(parseTree pt,int true_stmts, int is_else, int false_stmts);
void cgen_boolean2(parseTree pt);
void cgen_boolean1(parseTree pt1, parseTree pt2,char *op);
void cgen_size(parseTree pt);
void cgen_getvar_matrix(parseTree pt,int num_children);
void cgen_search_matrix(parseTree pt1,parseTree pt2 , parseTree pt3);

void call_on(parseTree pt)
{
	char *sym_string1 = symbols[pt->sym];
	char *sym_string = pt->t.token;
//	printf("sym_string at call_on %s\n",sym_string1);
	
	char *lex  = pt->t.lxme;
	// int index = 0;
	
//	printf("index = %d\n",index);
	if(strcmp(sym_string,"IF")==0)
	{
		//printf("in if call on \n");
		int true_stmts =0,false_stmts=0;
		int i;
		for(i = 1;i<pt->num_children;i++)
		{
			if(pt->children[i]->sym == find_ht(symbol_nos,"ELSE"))
				break;
			else
				true_stmts++;
		}
		int is_else =0;
		false_stmts = pt->num_children - 2 - true_stmts;
		if(false_stmts > 0)
			is_else = 1;
		// printf("here at t= %d f= %d num_c=%d\n",true_stmts,false_stmts,pt->num_children );
		cgen_ifstmt(pt->children[0],true_stmts,is_else,false_stmts);
		return;
	}
	else if((!strcmp(sym_string,"PLUS"))|| (!strcmp(sym_string,"MINUS")) || (!strcmp(sym_string,"MUL")) || (!strcmp(sym_string,"DIV")))
	{
		// if(!strcmp())
		// printf("call cgen_arithmetic\n");
		cgen_arithmetic(pt->children[0],sym_string,pt->children[1]);
		return;
	}
	else if(strcmp(sym_string,"ASSIGNOP") == 0)
	{
		cgen_assignment(pt->children[0],pt->children[1]);
		return;
	}

	else if(!(strcmp(pt->t.token,"ID"))|| !(strcmp(pt->t.token,"NUM"))||!(strcmp(pt->t.token,"STR"))|| !strcmp(pt->t.token,"RNUM"))
	{
		// symbol_table st = (symbol_table)(pt->sym_table_ptr);
		// int index = find_ht(st->sym_indices,lex);
		// char *type = st->table[index].type;
		cgen_getvar(pt);
		return;
	}
	else if(!strcmp(sym_string,"SIZE"))
	{
		cgen_size(pt->children[0]);
		return;
	}
	else if((!strcmp(sym_string,"GT"))||(!strcmp(sym_string,"LT"))||(!strcmp(sym_string,"LE"))||(!strcmp(sym_string,"GE"))||(!strcmp(sym_string,"EQ"))||(!strcmp(sym_string,"NE"))||(!strcmp(sym_string,"AND"))||(!strcmp(sym_string,"OR")))
	{
		cgen_boolean1(pt->children[0],pt->children[1],sym_string);
		return;
	}	
	else if((!strcmp(sym_string,"NOT")))
	{
		cgen_boolean2(pt->children[0]);
		return;
	}
	else if(!strcmp(sym_string1,"<matrix>"))
	{
		// printf("i am here\n");
		cgen_getvar_matrix(pt,pt->num_children);
		return;
	}
	else if(!strcmp(sym_string,"<Mat_Elem>"))
	{
		cgen_search_matrix(pt->children[0],pt->children[1],pt->children[2]);
		return;
	}
	else if((strcmp(sym_string,"PRINT")==0)||(strcmp(sym_string,"READ")==0))
	{
		// printf("here at read\n");
		cgen_io(pt->children[0],sym_string);
		return;
	}
	// else if() 		//TODO : boolean expression
}


void cgen_search_matrix(parseTree pt1,parseTree pt2 , parseTree pt3)
{

	call_on(pt3);
	fprintf(fp, "PUSH AX\n");
	call_on(pt2);
	fprintf(fp, "PUSH AX\n");
	call_on(pt1);
	fprintf(fp, "PUSH AX\n");
	// fprintf(fp, "PUSH BX\n");
	
	symbol_table st = (symbol_table)pt1->sym_table_ptr;

	char *lex = pt1->t.lxme;
	int index = find_ht(st->sym_indices,lex);
	int address =  st->base + st->table[index].offset;

	int col_size = st->table[index].s2;

	fprintf(fp, ";searching for element in the matrix\n");
	fprintf(fp, "POP SI\n");
	fprintf(fp, "POP AX\n");
	fprintf(fp, "POP BX\n");
	fprintf(fp, "MOV CX,%d\n",col_size);
	fprintf(fp,	"MUL CX\n");
	fprintf(fp,	"ADD AX,BX\n");
	fprintf(fp,	"ADD SI,AX\n");
	// fprintf(fp,	"SUB SI,1\n");
	fprintf(fp,	"MOV AX,[SI]\n");
	fprintf(fp,"\n");
	fprintf(fp,"\n");
}

void cgen_getvar_matrix2(parseTree pt)
{
	symbol_table st = (symbol_table)pt->sym_table_ptr;
	// char *sym_string = symbols[pt1->sym];
	char *lex = pt->t.lxme;
	int index = find_ht(st->sym_indices,lex);
	int address =  st->base + st->table[index].offset;
	fprintf(fp, ";getting pointer to first element of matrix\n" );
	fprintf(fp, "MOV AX,%d\n",address);
	int size = (st->table[index].s1) *(st->table[index].s2);
	fprintf(fp,"MOV BX,%d\n",size);
	return;
}
void cgen_getvar_matrix(parseTree pt,int num_children)
{
	int i,j;
	int count =0;
	fprintf(fp,";copying elements in buffer for copying later\n");
	fprintf(fp, "MOV SI,0050\n");
	
	for(i = 0;i<pt->num_children;i++)
	{
		parseTree row = pt->children[i];
		for(j = 0;j<row->num_children;j++)
		{

			count++;	
			fprintf(fp, "MOV BX,%d\n",row->children[j]->t.i_val);
			fprintf(fp, "MOV [SI],BX\n");
			fprintf(fp, "ADD SI,2\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}
	}
	fprintf(fp, "MOV AX,0050\n");
	fprintf(fp, "MOV BX,%d\n",count);
	return;

	
}
void cgen_io(parseTree pt,char *cmd)
{
	//printf("here\n");
	if_id++;
	int loc_if_id = if_id;
	// printf("sym = %d\n",pt->sym );
	// printf("token = %s\n",pt->t.token );
	symbol_table st = (symbol_table)(pt->sym_table_ptr);

	// printf("num sym %d\n",st->num_sym );
	char *lex  = pt->t.lxme;
	//printf("lexme = %s\n",lex );
	char *sym_string = symbols[pt->sym];
		// printf("sym_string = %s\n",sym_string);
	int index = find_ht(st->sym_indices, lex);
	// printf("index = %d\n",index );
	if(strcmp(cmd,"READ") == 0)
	{
		int address = st->base + st->table[index].offset;
		// printf("here1\n");
		// printf("type = %s\n",st->table[index].type );
		if(strcmp(st->table[index].type,"INT") == 0)
		{
			fprintf(fp,";reading an integer\n");
			fprintf(fp, "LEA DX,MAX1\n");
			fprintf(fp, "MOV AH,0AH\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "MOV CX,ACT1\n");
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "MOV BX,10\n");
			fprintf(fp, "MOV DX,0\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV DL,[SI+CX-1]\n");
			fprintf(fp, "SUB DL,30H\n");
			fprintf(fp, "ADD AX,DX\n");
			fprintf(fp, "MUL BX\n");
			// fprintf(fp, "INC SI\n");
			fprintf(fp, "DEC CX\n");
			fprintf(fp, "JNZ L1%d\n",loc_if_id);
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV [SI],AX\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}
		else if(strcmp(st->table[index].type,"STRING") == 0)
		{
			fprintf(fp,";reading a string\n");
			fprintf(fp, "LEA DX,MAX1\n");
			fprintf(fp, "MOV AH,0AH\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "MOV DI,%d\n",address);
			fprintf(fp, "MOV CX,ACT1\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV BL,[SI]\n");
			fprintf(fp, "MOV [DI],BL\n");
			fprintf(fp, "INC DI\n");
			fprintf(fp, "INC SI\n");
			fprintf(fp, "DEC CX\n");
			fprintf(fp, "JNZ L1:%d\n",loc_if_id);
			fprintf(fp, "MOV [DI],'~'\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}
		else if(strcmp(st->table[index].type,"REAL") == 0)
		{
			fprintf(fp,";reading a real number\n");
			fprintf(fp, "LEA DX,MAX1\n");	
			fprintf(fp, "MOV AH,0AH\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "MOV CX,ACT1\n");
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "MOV BX,10\n");
			fprintf(fp, "MOV DX,0\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV DL,[SI+CX-1]\n");
			fprintf(fp, "SUB DL,30H\n");
			fprintf(fp, "ADD AX,DX\n");
			fprintf(fp, "MUL BX\n");
			fprintf(fp, "DEC CX\n");
			fprintf(fp, "ADD DL,30H\n");
			fprintf(fp, "CMP [DL],'.'\n");
			fprintf(fp, "JNZ L1%d\n",loc_if_id);
			fprintf(fp, "\n");
			fprintf(fp, "DEC CX\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "L2%d:\n",loc_if_id);
			fprintf(fp, "MOV DL,[SI+CX-1]\n");
			fprintf(fp, "SUB DL,30H\n");
			fprintf(fp, "ADD AX,DX\n");
			fprintf(fp, "MUL BX\n");
			fprintf(fp, "DEC CX\n");
			fprintf(fp, "JNZ L2%d\n",loc_if_id);
			fprintf(fp, "POP BX\n");
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV [SI],BX\n");
			fprintf(fp, "ADD SI,2\n");
			fprintf(fp, "MOV [SI],AX\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");

		}
	}
	else
	{
		int address = st->base + st->table[index].offset;
		if(strcmp(st->table[index].type,"INT") == 0)
		{
			fprintf(fp,";printing an integer\n");
			fprintf(fp,"\n");
			fprintf(fp,"\n");
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV AX,[SI]\n");
			fprintf(fp, "MOV BX,10\n");
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV DX,0\n");
			fprintf(fp, "DIV BX\n");
			fprintf(fp, "ADD DL,30H\n");
			fprintf(fp, "MOV [SI],DL\n");
			fprintf(fp, "INC SI\n");
			fprintf(fp, "CMP AX,0\n");
			fprintf(fp, "JNZ L1%d\n",loc_if_id);
			fprintf(fp, "MOV [SI],'$'\n");
			fprintf(fp, "LEA DX,INP1\n");
			fprintf(fp, "MOV AH,09H\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}	
		else if(strcmp(st->table[index].type,"STRING") == 0)
		{
			fprintf(fp,";printing a string\n");
			fprintf(fp,"\n");
			fprintf(fp,"\n");
			fprintf(fp, "MOV DI,%d\n",address);
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "MOV BL,[DI]\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV [SI],BL\n");
			fprintf(fp, "INC DI\n");
			fprintf(fp, "INC SI\n");
			fprintf(fp, "MOV BL,[DI]\n");
			fprintf(fp, "CMP BL,'~'\n");
			fprintf(fp, "JNZ L1%d\n",loc_if_id);
			fprintf(fp, "MOV [SI],'$'\n");
			fprintf(fp, "LEA DX,INP1\n");
			fprintf(fp, "MOV AH,09H\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}
		else if(!strcmp(st->table[index].type,"REAL"))
		{
			fprintf(fp,";printing a real number\n");
			fprintf(fp,"\n");
			fprintf(fp,"\n");
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "ADD SI,2\n" );
			fprintf(fp, "MOV AX,[SI]\n");
			fprintf(fp, "MOV BX,10\n");
			fprintf(fp, "LEA SI,INP1\n");
			fprintf(fp, "L1%d:\n",loc_if_id);
			fprintf(fp, "MOV DX,0\n");
			fprintf(fp, "DIV BX\n");
			fprintf(fp, "ADD DL,30H\n");
			fprintf(fp, "MOV [SI],DL\n");
			fprintf(fp, "INC SI\n");
			fprintf(fp, "CMP AX,0\n");
			fprintf(fp, "JNZ L1%d\n",loc_if_id);

			fprintf(fp, "MOV [SI],'.'\n");
			fprintf(fp, "INC SI\n");

			fprintf(fp, "MOV DI,%d\n",address);
			fprintf(fp, "MOV AX,[DI]\n");
			fprintf(fp, "L2%d:\n",loc_if_id);
			fprintf(fp, "MOV DX,0\n");
			fprintf(fp, "DIV BX\n");
			fprintf(fp, "ADD DL,30H\n");
			fprintf(fp, "MOV [SI],DL\n");
			fprintf(fp, "INC SI\n");
			fprintf(fp, "CMP AX,0\n");
			fprintf(fp, "JNZ L2%d\n",loc_if_id);
			fprintf(fp, "\n");
			fprintf(fp, "MOV [SI],'$'\n");
			fprintf(fp, "LEA DX,INP1\n");
			fprintf(fp, "MOV AH,09H\n");
			fprintf(fp, "INT 21H\n");
			fprintf(fp, "\n");
			fprintf(fp, "\n");
		}
	}
}

void cgen_size(parseTree pt)
{
	symbol_table st = (symbol_table)pt->sym_table_ptr;
	// char *sym_string = symbols[pt1->sym];
	char *lex = pt->t.lxme;
	int index = find_ht(st->sym_indices,lex);
	int address =  st->base + st->table[index].offset;
	if(!strcmp(st->table[index].type,"STRING"))
	{
		fprintf(fp,";calculating size of string\n");
		fprintf(fp,"\n");
		fprintf(fp, "MOV AX,%d\n",st->table[index].s1);
		// fprintf(fp, "PUSH AX\n");
		return;
	}
	else if(!strcmp(st->table[index].type,"MATRIX"))
	{
		fprintf(fp,";calculating size of matrix\n");
		fprintf(fp, "MOV AX,%d\n",st->table[index].s1);
		// fprintf(fp,"PUSH AX\n");
		fprintf(fp,"MOV BX,%d\n",st->table[index].s2);
		// fprintf(fp,"PUSH AX\n");
		fprintf(fp,"\n");
		return;

	}
}



void cgen_assignment_matrix(parseTree pt1,parseTree pt2)
{
	symbol_table st1 = (symbol_table)pt1->sym_table_ptr;
	char *lex1 = pt1->t.lxme;
	int index1 = find_ht(st1->sym_indices,lex1);
	int address1 =  st1->base + st1->table[index1].offset;

	symbol_table st2 = (symbol_table)pt2->sym_table_ptr;
	char *lex2 = pt2->t.lxme;
	int index2 = find_ht(st2->sym_indices,lex2);
	int address2 =  st2->base + st2->table[index2].offset;

	fprintf(fp,";assigning a matrix\n");
	fprintf(fp, "MOV SI,%d\n",address1);
	fprintf(fp, "POP AX\n");
	fprintf(fp, "MOV [SI],AX\n");
	fprintf(fp, "MOV SI,%d\n",address2);
	fprintf(fp, "POP AX \n");
	fprintf(fp, "MOV [SI],AX\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	return;

}
void cgen_assignment(parseTree pt1 , parseTree pt2)
{

	call_on(pt2);
	
	fprintf(fp,";handling ASSIGNOP operation\n");
	fprintf(fp, "PUSH BX\n" );
	fprintf(fp, "PUSH AX\n");

	if(!strcmp(symbols[pt1->sym],"<leftHandSide_listVar>"))
	{
		cgen_assignment_matrix(pt1->children[0],pt1->children[1]);
		return;
	}
	if_id++;
	int loc_if_id = if_id;
	symbol_table st = (symbol_table)pt1->sym_table_ptr;
	char *sym_string = symbols[pt1->sym];
	char *lex = pt1->t.lxme;
	int index = find_ht(st->sym_indices,lex);
	int address =  st->base + st->table[index].offset;
	if(!strcmp(st->table[index].type,"MATRIX"))
	{
		fprintf(fp, "POP AX\n");
		fprintf(fp, "POP CX\n");		//count of elements
		fprintf(fp, "MOV SI,AX\n");
		fprintf(fp, "MOV DI,%d\n",address);
		fprintf(fp, "MATRIX_LABEL%d:\n",loc_if_id);
		fprintf(fp, "MOV BX,[SI]\n");
		fprintf(fp, "MOV [DI],BX\n");
		fprintf(fp, "ADD DI,2\n");
		fprintf(fp, "ADD SI,2\n");
		fprintf(fp, "DEC CX\n");
		fprintf(fp, "JNZ MATRIX_LABEL%d\n",loc_if_id);
		fprintf(fp, "\n");
		fprintf(fp, "\n");
	}
	else if(strcmp(st->table[index].type,"INT") == 0)
	{	
			// printf("ADDRESS%d BASE = %d\n",address,st->base);
			fprintf(fp, "POP AX\n");
			fprintf(fp, "POP BX\n" );
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV [SI],AX\n");
			fprintf(fp, "\n");
			return;
	}
	else if(strcmp(st->table[index].type,"STRING")== 0)
	{
		fprintf(fp, "POP DI\n");
		fprintf(fp, "POP BX\n" );
		fprintf(fp, "MOV SI,%d\n",address);
		fprintf(fp, "MOV BL,[DI]\n");
		fprintf(fp, "L1%d:\n",loc_if_id);
		fprintf(fp, "MOV [SI],BL \n");
		fprintf(fp, "INC SI\n");
		fprintf(fp, "INC DI\n");
		fprintf(fp, "MOV BL,[DI]\n");
		fprintf(fp, "CMP BL,'~'\n");
		fprintf(fp, "JNZ L1%d\n",loc_if_id);
		fprintf(fp, "MOV [SI],'~'\n");
		fprintf(fp, "\n");
	}
	else if(!strcmp(st->table[index].type ,"REAL"))
	{
		fprintf(fp, "POP AX\n" );
		fprintf(fp, "POP BX\n" );
		fprintf(fp, "MOV SI,%d\n",address );
		fprintf(fp, "MOV [SI],BX\n" );
		fprintf(fp, "ADD SI,2\n" );
		fprintf(fp, "MOV [SI],AX\n" );
		fprintf(fp, "\n" );
		fprintf(fp, "\n" );

	}

}

void cgen_getvar(parseTree pt)
{
	
	if(strcmp(pt->t.token,"ID") == 0)
	{
		symbol_table st = (symbol_table)pt->sym_table_ptr;
		char *sym_string = symbols[pt->sym];
		char *lex = pt->t.lxme;
		int index = find_ht(st->sym_indices,lex);
		int address = st->base + st->table[index].offset;
		// printf("ADDRESS%d BASE = %d\n",address,st->base);

		if(!strcmp(st->table[index].type,"INT"))
		{
			fprintf(fp, ";si pointing to offset of the variable\n" );
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV AX,[SI]\n");
			return;
		}
		else if(!strcmp(st->table[index].type,"MATRIX"))
		{
			cgen_getvar_matrix2(pt);
			return;
		}
		else if(!strcmp(st->table[index].type,"STRING"))
		{
			fprintf(fp, "MOV AX,%d\t;AX pointing to the first position of string\n",address);
			return;
		}
		else if(!strcmp(st->table[index].type,"REAL"))
		{
			//TODO : handle real numbers
			fprintf(fp, "MOV SI,%d\n",address);
			fprintf(fp, "MOV BX,[SI]\n" );
			fprintf(fp, "ADD SI,2\n" );
			fprintf(fp, "MOV AX,[SI]\n" );
			fprintf(fp, "\n" );
			fprintf(fp, "\n" );
			fprintf(fp, "\n" );
			fprintf(fp, "\n" );
		}
	}
	else if(!strcmp(pt->t.token,"NUM"))
	{

		int val = pt->t.i_val;
		fprintf(fp, "MOV AX,%d\n",val );
		fprintf(fp, "\n" );
		fprintf(fp, "\n" );
		return;
		
	}
	else if(!strcmp(pt->t.token,"STR"))
	{
		char *val = pt->t.lxme;
		int i = 1;
		fprintf(fp, "LEA SI,INP1\n" );
		while(val[i]!='"')
		{
			
			fprintf(fp, "MOV [SI],'%c'\n",val[i]);
			fprintf(fp, "INC SI\n" );
			i++;
			
		}
		fprintf(fp, "\n" );
		fprintf(fp, "MOV [SI],'~'\n");
		fprintf(fp, "LEA AX,INP1\n" );
		fprintf(fp, "\n");
		fprintf(fp, "\n");
		return;
	}
	else if(!strcmp(pt->t.token,"RNUM"))
	{

		// printf("hey token is RNUM\n");
		float num = pt->t.r_val;
		// printf("num = %f\n",pt->t.r_val );
		int fraction = (int)(num*100);
		int fraction_to_int = fraction%100;
		int non_fraction = (fraction - fraction_to_int)/100;
		// printf("non_fr = %d\n",non_fraction);
		fprintf(fp, "MOV AX,%d\n",non_fraction);
		fprintf(fp, "MOV BX,%d\n",fraction_to_int);

	}
}

void cgen_arithmetic_string(parseTree pt1,parseTree pt2)
{	

	
	call_on(pt1);		//will get the offset of pt1 stirng stored in AX
	fprintf(fp, "PUSH AX\n");
	fprintf(fp, "POP DI\n");
	fprintf(fp, "MOV SI,0050\n");
	if_id++;
	int loc_if_id = if_id;
	fprintf(fp, ";concatenation of string\n" );
	fprintf(fp, "L1%d:\n",loc_if_id);
	fprintf(fp, "MOV AL,[DI]\n");
	fprintf(fp, "MOV [SI],AL\n");
	fprintf(fp, "INC SI\n");
	fprintf(fp, "INC DI\n");
	fprintf(fp, "CMP [DI],'~'\n");
	fprintf(fp, "JNZ L1%d\n",loc_if_id);
	fprintf(fp, "PUSH SI\n");
	fprintf(fp, "\n");

	call_on(pt2);
	
	fprintf(fp, "PUSH AX\n");
	fprintf(fp, "POP DI\n");
	fprintf(fp, "POP SI\n");
	if_id++;
	loc_if_id = if_id;
	fprintf(fp, "L1%d:\n",loc_if_id);
	fprintf(fp, "MOV AL,[DI]\n");
	fprintf(fp, "MOV [SI],AL\n");
	fprintf(fp, "INC SI\n");
	fprintf(fp, "INC DI\n");
	fprintf(fp, "CMP [DI],'~'\n");
	fprintf(fp, "JNZ L1%d\n",loc_if_id);
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	fprintf(fp, "mov [SI],'~'\n" );
	fprintf(fp, "mov AX,0050\n" );
	fprintf(fp, "\n\n\n");
}
void cgen_arithmetic(parseTree pt1, char *op, parseTree pt2)
{
	
	if_id++;
	int loc_if_id = if_id;
	if(!strcmp(pt1->t.token,"ID"))
	{
		symbol_table st = (symbol_table)pt1->sym_table_ptr;
		char *lex = pt1->t.lxme;
		int index = find_ht(st->sym_indices,lex);
		if(!strcmp(st->table[index].type,"STRING"))
		{
			cgen_arithmetic_string(pt1,pt2);
			return;
		}
	}
	else if(!strcmp(pt1->t.token,"STR"))
	{
		cgen_arithmetic_string(pt1,pt2);
		return;
	}

	call_on(pt1);			//TODO : Define call_on funtion which recursivlely decide on which function to call
	fprintf(fp,"PUSH AX\n");
	fprintf(fp, "PUSH BX\n" );
	call_on(pt2);
	fprintf(fp, "PUSH AX\n");
	fprintf(fp, "PUSH BX\n");

	fprintf(fp, ";arithematic operations\n" );
	if(!strcmp(pt1->t.token,"ID"))
	{
		
		symbol_table st = (symbol_table)pt1->sym_table_ptr;
		char *lex = pt1->t.lxme;
		int index = find_ht(st->sym_indices,lex);
		if(!strcmp(st->table[index].type,"MATRIX"))
		{
				fprintf(fp, "POP CX\n");
				fprintf(fp, "POP DI\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP SI\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				if(!strcmp(op,"PLUS"))
				{
					fprintf(fp, "MOV BX,0050\n");
					fprintf(fp, "MOV DX,CX\n");
					fprintf(fp, "L1%d:\n",loc_if_id);
					fprintf(fp, "MOV AX,[DI]\n");
					fprintf(fp, "ADD AX,[SI]\n");
					fprintf(fp, "MOV [BX],AX\n");
					fprintf(fp, "ADD DI,2\n");
					fprintf(fp, "ADD SI,2\n");
					fprintf(fp, "ADD BX,2\n");
					fprintf(fp, "DEC CX\n");
					fprintf(fp, "JNZ L1%d\n",loc_if_id);
					fprintf(fp, "MOV AX,0050\n");
					fprintf(fp, "MOV BX,DX\n");
					fprintf(fp, "\n");
					fprintf(fp, "\n");	
				}
				else if(!strcmp(op,"MINUS"))
				{
					fprintf(fp, "MOV BX,0050\n");
					fprintf(fp, "MOV DX,CX\n");
					fprintf(fp, "L1%d:\n",loc_if_id);
					fprintf(fp, "MOV AX,[SI]\n");
					fprintf(fp, "SUB AX,[DI]\n");
					fprintf(fp, "MOV [BX],AX\n");
					fprintf(fp, "ADD DI,2\n");
					fprintf(fp, "ADD SI,2\n");
					fprintf(fp, "ADD BX,2\n");
					fprintf(fp, "DEC CX\n");
					fprintf(fp, "JNZ L1%d\n",loc_if_id);
					fprintf(fp, "MOV AX,0050\n");
					fprintf(fp, "MOV BX,DX\n");
					fprintf(fp, "\n");
					fprintf(fp, "\n");	
				}
		}

		else if(!strcmp(st->table[index].type,"REAL"))
		{
			if(!strcmp(op,"PLUS"))
			{
				fprintf(fp, "POP BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

				fprintf(fp, "ADD BX,CX\n");
				fprintf(fp, "CMP BX,100\n");
				fprintf(fp, "JL L1%d\n",loc_if_id);
				fprintf(fp, "ADD AX,1\n");
				fprintf(fp, "SUB BX,100\n");
				fprintf(fp, "L1%d:\n",loc_if_id);
				fprintf(fp, "ADD AX,DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");	
				fprintf(fp, "\n");
			

			}
			else if(!strcmp(op,"MINUS"))
			{

				fprintf(fp, "POP BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

				fprintf(fp, "SUB CX,BX\n");
				fprintf(fp, "CMP CX,0\n");
				fprintf(fp, "JGE L1%d\n",loc_if_id);
				fprintf(fp, "SUB DX,1\n");
				fprintf(fp, "ADD CX,100\n");
				fprintf(fp, "L1%d:\n",loc_if_id);
				fprintf(fp, "SUB DX,AX\n");
				fprintf(fp, "MOV BX,CX\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
			}
			else if(!strcmp(op,"MUL"))
			{
				fprintf(fp, "POP SI\n");
				fprintf(fp, "POP DI\n");
				fprintf(fp, "POP CX \n");
				// fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "MOV AX,CX\n");
				fprintf(fp, "MOV BX,SI\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "MOV BX,100\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "ADD AX,DI\n");
				fprintf(fp, "MOV BX,DX\n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "ADD AX,DX\n");
				fprintf(fp, "\n");
			}
			else if(!strcmp(op,"DIV"))
			{
				fprintf(fp, "POP SI\n");
				fprintf(fp, "POP DI\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "MOV BX 100\n");
				fprintf(fp, "MOV AX,DI\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "ADD AX,SI\n");
				fprintf(fp, "MOV DI,AX\n");

				fprintf(fp, "MOV AX,DX \n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "ADD AX,CX\n");
				fprintf(fp, "MOV BX,DI\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "PUSH AX\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "MOV BX,100\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "MOV BX,DI\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "MOV AX,BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

			}
		}
		else if(!strcmp(st->table[index].type,"INT"))
		{
			fprintf(fp, "POP BX\n");
			fprintf(fp, "POP BX\n");  
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP AX\n");

			if(!strcmp(op,"PLUS"))
			{
				fprintf(fp, "ADD AX,BX\n");
			}
			else if(!strcmp(op,"MINUS"))
			{
				fprintf(fp, "SUB AX,BX\n");
			}	
			else if(!strcmp(op,"MUL"))
			{
				fprintf(fp, "MUL BX\n");
			}
			else if(!strcmp(op,"DIV"))
			{
				fprintf(fp, "mov dx,0\n");
				fprintf(fp,"DIV BX\n");
				fprintf(fp, "PUSH AX\n");
				fprintf(fp, "MOV CX,100\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "MUL CX\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "MOV BX,AX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
			}
		}
	}
	else if(!strcmp(pt1->t.token,"RNUM"))
	{
		if(!strcmp(op,"PLUS"))
			{
				fprintf(fp, "POP BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

				fprintf(fp, "ADD BX,CX\n");
				fprintf(fp, "CMP BX,100\n");
				fprintf(fp, "JL L1%d\n",loc_if_id);
				fprintf(fp, "ADD AX,1\n");
				fprintf(fp, "SUB BX,100\n");
				fprintf(fp, "L1%d:\n",loc_if_id);
				fprintf(fp, "ADD AX,DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");	
				fprintf(fp, "\n");
			

			}
			else if(!strcmp(op,"MINUS"))
			{

				fprintf(fp, "POP BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

				fprintf(fp, "SUB CX,BX\n");
				fprintf(fp, "CMP CX,0\n");
				fprintf(fp, "JGE L1%d\n",loc_if_id);
				fprintf(fp, "SUB DX,1\n");
				fprintf(fp, "ADD CX,100\n");
				fprintf(fp, "L1%d:\n",loc_if_id);
				fprintf(fp, "SUB DX,AX\n");
				fprintf(fp, "MOV BX,CX\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
			}
			else if(!strcmp(op,"MUL"))
			{
				fprintf(fp, "POP SI\n");
				fprintf(fp, "POP DI\n");
				fprintf(fp, "POP CX \n");
				// fprintf(fp, "POP DX\n");
				fprintf(fp, "\n");
				fprintf(fp, "MOV AX,CX\n");
				fprintf(fp, "MOV BX,SI\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "MOV BX,100\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "ADD AX,DI\n");
				fprintf(fp, "MOV BX,DX\n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "ADD AX,DX\n");
				fprintf(fp, "\n");
			}
			else if(!strcmp(op,"DIV"))
			{
				fprintf(fp, "POP SI\n");
				fprintf(fp, "POP DI\n");
				fprintf(fp, "POP CX \n");
				fprintf(fp, "POP DX\n");
				fprintf(fp, "MOV BX 100\n");
				fprintf(fp, "MOV AX,DI\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "ADD AX,SI\n");
				fprintf(fp, "MOV DI,AX\n");

				fprintf(fp, "MOV AX,DX \n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "ADD AX,CX\n");
				fprintf(fp, "MOV BX,DI\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "PUSH AX\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "MOV BX,100\n");
				fprintf(fp, "MUL BX\n");
				fprintf(fp, "MOV BX,DI\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "MOV AX,BX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");

			}
	}
	else if(!strcmp(pt1->t.token,"NUM"))
	{
		fprintf(fp, "POP BX\n");
			fprintf(fp, "POP BX\n");  
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP AX\n");

			if(!strcmp(op,"PLUS"))
			{
				fprintf(fp, "ADD AX,BX\n");
			}
			else if(!strcmp(op,"MINUS"))
			{
				fprintf(fp, "SUB AX,BX\n");
			}	
			else if(!strcmp(op,"MUL"))
			{
				fprintf(fp, "MUL BX\n" );
			}
			else if(!strcmp(op,"DIV"))
			{
				fprintf(fp, "mov dx,0\n");
				fprintf(fp,"DIV BX\n");
				fprintf(fp, "PUSH AX\n");
				fprintf(fp, "MOV CX,100\n");
				fprintf(fp, "MOV AX,DX\n");
				fprintf(fp, "MUL CX\n");
				fprintf(fp, "mov dx,0\n");
				fprintf(fp, "DIV BX\n");
				fprintf(fp, "MOV BX,AX\n");
				fprintf(fp, "POP AX\n");
				fprintf(fp, "\n");
				fprintf(fp, "\n");
			}
	}
	return;
}


void cgen_boolean2(parseTree pt)
{
	call_on(pt);
	if_id++;
	int loc_if_id = if_id;
	fprintf(fp, ";not expression\n" );
	fprintf(fp,"CMP AX,1\n");
	fprintf(fp, "JZ FALSE_LABEL%d\n",loc_if_id);
	fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
	fprintf(fp, "MOV AX,1\n");
	fprintf(fp, "PUSH AX\n");
	fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
	fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
	fprintf(fp, "MOV AX,0\n");
	fprintf(fp, "PUSH AX\n");
	fprintf(fp, "END_LABEL%d:\n",loc_if_id);
	fprintf(fp, "\n");
	return;
}
void cgen_boolean1(parseTree pt1, parseTree pt2,char *op)
{
	call_on(pt1);
	fprintf(fp, "PUSH AX\n");
	fprintf(fp,"PUSH BX\n");

	call_on(pt2);
	fprintf(fp, "PUSH AX\n");
	fprintf(fp,"PUSH BX\n");
	fprintf(fp, ";boolean expression\n" );
	if(!strcmp(op,"AND")||!strcmp(op,"OR"))
	{
		fprintf(fp, "POP BX\n");
		fprintf(fp, "POP BX\n");
		fprintf(fp, "POP AX\n");
		fprintf(fp, "POP AX\n");
		if_id++;
		int loc_if_id = if_id;
		if(!strcmp(op,"AND"))
		{
			fprintf(fp, "CMP BX,1\n");
			fprintf(fp, "JNZ FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP AX,1\n");
			fprintf(fp, "JNZ FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
		else if(!strcmp(op,"OR"))
		{
			fprintf(fp, "CMP BX,1\n");
			fprintf(fp, "JZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP AX,1\n");
			fprintf(fp, "JZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;

		}
	}
	if(!strcmp(pt1->t.token,"NUM") || !strcmp(pt1->t.token,"INT"))
	{
		fprintf(fp, "POP BX\n");
		fprintf(fp, "POP BX\n");
		fprintf(fp, "POP AX\n");
		fprintf(fp, "POP AX\n");
		if_id++;
		int loc_if_id = if_id;
		if(!strcmp(op,"GT"))
		{
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JG TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}

		else if(!strcmp(op,"LT"))
		{
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JL TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			//fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}

		else if(!strcmp(op,"LE"))
		{
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JLE TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}

		else if(!strcmp(op,"GE"))
		{
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JGE TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
		else if(!strcmp(op,"EQ"))
		{	
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;

		}
		else if(!strcmp(op,"NE"))
		{
			fprintf(fp, "CMP AX,BX\n");
			fprintf(fp, "JNZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "PUSH AX\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
	}
	else if(!strcmp(pt1->t.token,"RNUM") || !strcmp(pt1->t.token,"REAL"))
	{
		fprintf(fp, "POP DX\n");
		fprintf(fp, "POP CX\n");
		fprintf(fp, "POP SI\n");
		fprintf(fp, "POP DI\n");
		if_id++;
		int loc_if_id = if_id;
		if(!strcmp(op,"GT"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JG TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JZ EQUAL_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JG TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "JMP FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;

		}
		else if(!strcmp(op,"LT"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JL TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JZ EQUAL_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JL TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "JMP FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}

		else if(!strcmp(op,"LE"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JL TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JZ EQUAL_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JLE TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "JMP FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;

		}
		else if(!strcmp(op,"GE"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JG TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JZ EQUAL_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JGE TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "JMP FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
		else if(!strcmp(op,"EQ"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JZ EQUAL_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "JMP FALSE_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
		else if(!strcmp(op,"NE"))
		{
			fprintf(fp, "CMP DI,CX\n");
			fprintf(fp, "JNZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "EQUAL_LABEL%d:\n",loc_if_id);
			fprintf(fp, "CMP DX,SI\n");
			fprintf(fp, "JNZ TRUE_LABEL%d\n",loc_if_id);
			fprintf(fp, "FALSE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,0\n");
			fprintf(fp, "JMP END_LABEL%d\n",loc_if_id);
			fprintf(fp, "TRUE_LABEL%d:\n",loc_if_id);
			fprintf(fp, "MOV AX,1\n");
			fprintf(fp, "END_LABEL%d:\n",loc_if_id);
			fprintf(fp, "\n");
			return;
		}
			
	}
}

void cgen_ifstmt(parseTree pt,int true_stmts, int is_else, int false_stmts)
{
	//printf("true= %d false = %d\n",true_stmts,false_stmts);
	if_id++;
	int loc_if_id = if_id;
	int i;
	parseTree parent = pt->parent;
	call_on(pt);
	fprintf(fp, "POP AX\n");
	fprintf(fp, ";if stmt\n" );
	fprintf(fp, "CMP AX,1\n");
	fprintf(fp, "JZ true_label%d\n",loc_if_id);	
	if(is_else)
	{
		fprintf(fp, "false_label%d:\n",loc_if_id);
		for(i =2+true_stmts;i<parent->num_children;i++)
			call_on(parent->children[i]);
		fprintf(fp, "JMP endif_label%d\n",loc_if_id);
	}
	fprintf(fp, "true_label%d:\n",loc_if_id);
	for(i = 1;i<=true_stmts;i++)
	{
		call_on(parent->children[i]);
	}
	 // fprintf(fp, "JMP end_if%d\n",if_id);
	fprintf(fp, "endif_label%d:\n",loc_if_id );
	return;
}

void code_generation(parseTree pt, char *out_file)
{
	fp = fopen(out_file,"w");
	write_header();
	int i;
	for(i = 0;i<pt->num_children;i++)
	{
		call_on(pt->children[i]);
	}
	wrtie_footer();
}
