/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: lexer.c

********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "lexerDef.h"

FILE *fp;
buffer buf;
int chars_in_buf; //it will be equal to BUF_SIZE, unless EOF has been reached
int buf_pointer = 0;
int cur_line = 1;

FILE *getStream(FILE *fp1, buffer B, buffersize k)
{
	chars_in_buf = (int) fread(B,sizeof(char),BUF_SIZE,fp1);
	return fp1;
}

tokenInfo epsilon_token(){                    
   tokenInfo t;
   strcpy(t.token,"EPSILON"); 
   t.line_no = 0;               
   strcpy(t.lxme,"");                         
   return t;                         
}

void open_file(const char *filename){
	fp = fopen(filename,"r");
	if(fp==NULL){
		printf("Could not open file %s\n",filename);
	}
	fp = getStream(fp,buf,BUF_SIZE);
}

void close_file(){
	fclose(fp);
}

int str_to_int(char *str)
{
	int l = strlen(str), i, result = 0;
	for(i=0;i<l;i++)
	{
		result = result*10 + (str[i] - '0');
	}
	return result;
}

float str_to_real(char *str)
{
	int l = strlen(str), i;
	assert(str[l-3]=='.');
	float result = 0.0;
	//assuming two digits after the decimal point
	for(i=0;i<l-3;i++)
	{
		result = result*(10.0) + (float) (str[i] - '0');
	}
	result += ( (float) str[l-2]-'0')/10.0 + ( (float) str[l-1] - '0')/100.0;
	return result;
}

DFA read_dfa(const char *filename, Hashtable ht, char symbols[][25], int *num)
{
	//reads DFA from specified file, updates info about the terminals
	FILE *fp = fopen(filename,"r");
	if(fp==NULL){
		printf("File %s could not be opened\n",filename);
		exit(0);
	}
	DFA d;
	fscanf(fp,"%d\n",&(d.num_states));
	fscanf(fp,"%d\n",&(d.start_state));
	int i, num_terminals = 1;
	for(i=1;i<=d.num_states;i++){
		//info about each state
		// state_no <space> final?(Y/N) <space> token(if final) <space> backtrack? (Y/N, if final)
		int s_no;
		char temp[25], tok[25];
		fscanf(fp,"%d %s",&s_no,temp);
		if(!strcmp(temp,"N")){
			d.is_final[s_no] = 0;
		}
		else d.is_final[s_no] = 1;
		if(d.is_final[s_no]){
			fscanf(fp, "%s %s",tok,temp);
			strcpy(d.tok[s_no],tok);
			if(find_ht(ht,tok)==-1){
				strcpy(symbols[num_terminals], tok);
				insert_ht(ht,tok,num_terminals++);
			}
			if(!strcmp(temp,"N")){
				d.to_backtrack[s_no] = 0;
			}
			else d.to_backtrack[s_no] = 1;
		}
	}
	memset(d.transitions,-1,sizeof(d.transitions)); //if not specified -> invalid
	//info about transitions - one per line till EOF
	//from_state <space> to_state <space> range of chars (eg. a z)

	char r1[10], r2[10];
	int t1, t2;
	while(!feof(fp)){
		fscanf(fp,"%d %d %s",&t1,&t2,r1);
		if(!strcmp(r1,"others")){
			int i;
			for(i=0;i<128;i++){
				if(d.transitions[t1][i] == -1)
					d.transitions[t1][i] = t2;
			}
		}
		else{
			fscanf(fp,"%s",r2);
			if(!strcmp(r1,"sp"))
				r1[0] = ' ';
			if(!strcmp(r2,"sp"))
				r2[0] = ' ';

			int i;
			for(i=r1[0];i<=r2[0];i++){
				d.transitions[t1][i] = t2;
			}
		}
	}

	*num = num_terminals;
	fclose(fp);
	return d;
}

int is_unknown(char c){
	if(c>128)
		return 1;
	if(c=='^' || c=='~' || c=='`' || c==':' || c=='$' || c=='%' || c=='&' || c=='|')
		return 1;
	return 0;
}

tokenInfo  getNextToken(  DFA *d)
{
	char lxm[100];
	tokenInfo t;
	int cur_state = d->start_state, lxm_size = 0;

	int started = 0, comment =0;
	//cur_state = -1 represents invalid state
	char c;
	while(cur_state>=0 && !d->is_final[cur_state])
	{
		if(buf_pointer == chars_in_buf)
		{
			if(chars_in_buf<BUF_SIZE){
				//EOF reached
				strcpy(t.lxme,"$");
				strcpy(t.token,"$");
				return t;
			}
			else{
				buf_pointer = 0;
				fp = getStream(fp, buf, BUF_SIZE);
			}
		}
		c = buf[buf_pointer++];
		if(!comment && c>128){
			cur_state = -1;
			break;
		}
		//ignore comments and whitespaces
		if(!started){
			if(c>32 && c!='#' && !comment){
				started = 1;
			}
			else{
				if(c=='#')
					comment = 1;
				if(c=='\n'){
					comment = 0;
					cur_line++;
				}
				continue;
			}
		}
		cur_state = d->transitions[cur_state][c];
		lxm[lxm_size++] = c;
	}
	
	t.line_no = cur_line;

	if(cur_state==-1)
	{
		if(is_unknown( c)){
			printf("Unknown symbol '%c' at line %d.\n",c,cur_line);	
		}
		else{
			printf("Unknown pattern %s at line %d.\n",lxm,cur_line);
		}
		strcpy(t.token,"INVALID");
		strncpy(t.lxme,lxm,25);
		return t;
	}

	else{
		//in Final state
		if(d->to_backtrack[cur_state])
		{
			buf_pointer--;
			lxm_size--;
		}
		lxm[lxm_size++] = '\0';
		if(!strcmp(d->tok[cur_state],"ID")){
			if(lxm_size > 20){
				printf("Identifier %s at line %d is longer than the prescribed length of 20 characters\n",lxm,cur_line);
			}
		}
		if(!strcmp(d->tok[cur_state],"STR")){
			if(lxm_size > 22){
				printf("String %s at line %d is longer than the prescribed length of 20 characters\n",lxm,cur_line);
			}
		}
		strcpy(t.token,d->tok[cur_state]);
		fflush(stdout);
		if(!strcmp(t.token,"NUM"))
			t.i_val = str_to_int(lxm);
		if(!strcmp(t.token,"RNUM"))
			t.r_val = str_to_real(lxm);
		strncpy(t.lxme,lxm,25);
		t.lxme[25] = '\0';
		fflush(stdout);
		return t;
	}
}

