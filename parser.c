/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: parser.c

********************/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "parserDef.h"
#include "stack.h"

//the first num_terminals symbols are terminals,
// rest are non-terminals


int cyc_id = 1;				//for taking care of cycles in follow set
int is_null_derivable[100],isVisited[100],isCycle[100],cycles[100];
sets first_set ;
sets follow_set;

int is_terminal(int symbol){
	return (symbol < num_terminals);
}


Tnode * new_tnode(int symbol, Tnode *par){
	Tnode * n 			 = (Tnode *) malloc(sizeof(Tnode));
	n -> sym 			 = symbol;
	n -> num_children  = 0;
	n -> parent 		 = par;
	n -> is_leaf 		 = is_terminal(symbol); 
	n -> children_seen = 0;
	n -> is_root 		 = 0;
	return n;
}

void read_grammar(){
	symbol_nos = new_HT(NUM_SYMBOLS*4);
	d = read_dfa("dfa.txt",symbol_nos,symbols,&num_terminals);
	strcpy(symbols[0],"EPSILON");
	insert_ht(symbol_nos,"EPSILON",0);
	g = (grammar) malloc(NUM_RULES*sizeof(grammar_rule));
	strcpy(symbols[num_terminals],"$");
	insert_ht(symbol_nos,"$",num_terminals++);
	start_symbol = num_symbols = num_terminals;
	FILE *Fp = fopen("grammar.txt","r");
	if(Fp == NULL )
		//exit(EXIT_FAILURE);
		printf("fp == null\n");

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int count,index = 0;
	int left;
	
	while((read =getline(&line,&len,Fp)) != -1)		//reading line by line of grammar
	{
		count = 0;
		char *elem = strtok(line," ");
		
		while(elem != NULL )
		{

			if(elem[0]<=32)
			{	
				elem = strtok(NULL," ");
				continue;
			}
			
			
			char t[30];
			sscanf(elem,"%s",t);

			if(count==0)
			{	
				//LHS of the grammar rule
				int code = find_ht(symbol_nos,t);
				if(code == -1)
				{
					
					insert_ht(symbol_nos,t,num_symbols);
					strcpy(symbols[num_symbols],t);
					left = num_symbols++;
				}
				else
					left = code;
				g[index].lhs = left;
			}
			else
			{
				//RHS of the grammar rule
				int code = find_ht(symbol_nos,t);
				if(code == -1)
				{
					insert_ht(symbol_nos,t,num_symbols);
					strcpy(symbols[num_symbols],t);
					g[index].rhs = insert_ll(g[index].rhs,num_symbols++);
				}
				else
					g[index].rhs = insert_ll(g[index].rhs,code);
			}
			elem = strtok(NULL," ");
			count++;
		}
		index++;
	}
	num_rules = index;
}


void firstSetTerminals(grammar first_set)
{
	int i;
	for(i = 0;i<=num_terminals;i++)
	{
		first_set[i].lhs = i;
		first_set[i].rhs = insert_ll(first_set[i].rhs,i);
		isVisited[i] = 1;
	}
}
void firstSetNonTerminals(grammar G,int rule, int *s, int tag)
{
	int i;
	int sym[100];
	if(tag == 0)
	{
		memset(sym,0,sizeof(sym));
	}
	else
	{
		for(i = 0;i<100;i++)
			sym[i] = s[i];
	}
	int left = G[rule].lhs,right_val;
	Node *head = G[rule].rhs.head;
	while(head!=NULL)
	{	
		right_val = head->val;
		if(isVisited[right_val])
		{
			Node *right_head = first_set[right_val].rhs.head;
			while(right_head!=NULL)
			{
				sym[right_head->val] = 1;
				right_head = right_head->next;
			}
			if(sym[0] ==1)
			{
				sym[0] = 0;
				head = head->next;
				continue;
			}
			else 
				break;
		}
		else
		{
			for(i = 0;i<num_rules;i++)
			{
				if(G[i].lhs == right_val)
				{
					firstSetNonTerminals(G,i,sym,0);
					break;
				}
			}
			Node *right_head = first_set[right_val].rhs.head;
			while(right_head!=NULL)
			{
				sym[right_head->val] = 1;
				right_head = right_head->next;
			}
			if(sym[0] == 1)
			{
				sym[0] = 0;
				head = head->next;
				continue;
			}
			else
				break;
		}
	}
	if(head == NULL)
	{
		if(find_ll(first_set[left].rhs,0) == 0)
			first_set[left].rhs = insert_ll(first_set[left].rhs,0);
	}

	if(G[rule+1].lhs == left)
		firstSetNonTerminals(G,rule+1,sym,1);
	else
	{
		first_set[left].lhs = left;
		for(i = 0;i<100;i++)
		{
			if(sym[i])
				first_set[left].rhs = insert_ll(first_set[left].rhs,i);
		}
		isVisited[left] = 1;
	}

}

void createFirstSet(grammar G)
{
	int i;
	first_set = (grammar)malloc(NUM_RULES*sizeof(grammar_rule));
	int sym[100];
	memset(isVisited,0,sizeof(isVisited));
	firstSetTerminals(first_set);
	for(i = 0;i<num_rules;i++)
	{
		int left = G[i].lhs;
		if(!isVisited[left])
		{
			memset(sym,0,sizeof(sym));
			firstSetNonTerminals(G,i,sym,1);
		}
	}
}

void insertIntoSymbols(linked_list ll, int *sym)
{
	Node *head = ll.head;
	while(head != NULL)
	{
		sym[head->val] = 1;
		head =head->next;
	}
}


void insertFollowSet(int left , linked_list ll)
{
	if(follow_set[left].rhs.head==NULL)
	{
		Node *head = ll.head;
		while(head != NULL)
		{
			if(head->val!=0)
				
				follow_set[left].rhs = insert_ll(follow_set[left].rhs,head->val);
			head = head->next;
		}
	}
	else
	{
		Node *head = ll.head;
		while(head != NULL)
		{	
			if(head->val!=0)
			{
				int s = find_ll(follow_set[left].rhs,head->val); 
				if(s==0)											// s==0 means element is not present in list
				{
					follow_set[left].rhs = insert_ll(follow_set[left].rhs,head->val);
				}
			}
			head = head->next;
		}
	}
}

void insertFollowSet2(int left,int *sym)
{
	int i;
	for(i = 0;i<100;i++)
	{
		if(sym[i] == 1)
		{
			int s = find_ll(follow_set[left].rhs,i);
			if(s==0)
			{
				follow_set[left].rhs = insert_ll(follow_set[left].rhs,i);
			}
		}
	}
}

void followSet(grammar G,int left)
{
	fflush(stdout);
	int i;
	if(isCycle[left] == 1)
	{
		cycles[left] = cyc_id++;
		return;
	}
	follow_set[left].lhs = left;
	for(i = 0;i<num_rules;i++)
	{
		Node *head = G[i].rhs.head;
		while(head!=NULL)
		{
			if(head->val == 0)
				break;
			if(head->val == left)
			{
				head = head->next;
				while(head != NULL)
				{
					fflush(stdout);
					insertFollowSet(left,first_set[head->val].rhs);
					if(first_set[head->val].rhs.head->val == 0)
					{
						head = head->next;
						continue;
					}
					else
						break;

				}
				if(head == NULL)
				{
					if(G[i].lhs==left)
					{
						break;
					}
					if(isVisited[G[i].lhs])									
						insertFollowSet(left,follow_set[G[i].lhs].rhs);	
					else
					{
						isCycle[left] = 1;
						followSet(G,G[i].lhs);
						if(cycles[G[i].lhs] > 0)
						{
							cycles[left] = cycles[G[i].lhs];

						}
						isCycle[left] = 0;
						insertFollowSet(left,follow_set[G[i].lhs].rhs);
					}
				}

			}
			if(head != NULL)
			head = head->next;
		}
	}
	isVisited[left] =1;
}


void createFollowSet(grammar G)
{
	follow_set = (grammar)malloc(NUM_RULES*sizeof(grammar_rule));
	memset(isVisited,0,sizeof(isVisited));	
	int sym[100];
	int i,left,j;
	for(i=0;i<num_rules;i++)
	{
		left = G[i].lhs;
		if(!isVisited[left])
		{
			followSet(G,left);
		}
	}
	
	for(i = 1;i<cyc_id;i++)
	{
		memset(sym,0,sizeof(sym));
		for(j = 0;j<num_symbols;j++)
		{
			if(cycles[j] == i)
			{
				insertIntoSymbols(follow_set[j].rhs,sym);
			}
		}
		for(j = 0;j<num_symbols;j++)
		{
			if(cycles[j] == i)
			{
				insertFollowSet2(j,sym);		
			}
		}
	}
}


void createParseTable(grammar G,table T)
{
	createFirstSet(G);
	createFollowSet(G);
	int null_derivable = 0,i;
	for(i = 0;i<num_rules;i++)
	{
		int left = G[i].lhs;
		Node *head = G[i].rhs.head;
		while(head != NULL)
		{
			if(head-> val ==0)
			{
				Node *symbol_head = follow_set[left].rhs.head;
				while(symbol_head != NULL)
				{
					T[left][symbol_head->val] = i;		
					symbol_head = symbol_head->next;
				}
				head = head->next;
			}
			else
			{
				Node *symbol_head = first_set[head->val].rhs.head;
				while(symbol_head != NULL)
				{
					if(symbol_head->val == 0)
						null_derivable=1;
					else
					{
						T[left][symbol_head->val] = i;
						symbol_head = symbol_head->next;
					}
				}
				if(null_derivable == 0)
					break;
				else
				{
					null_derivable = 0;
					head = head->next;
					continue;
				}
			}
		}
	}
}



parseTree  parseInputSourceCode(char *testcaseFile, table T){
	open_file(testcaseFile);
	parseTree PT 	= new_tnode(start_symbol,NULL); 
	PT -> is_root = 1;
	PT -> sym 	  = start_symbol;
	stack s;
	s = push(s,start_symbol);
	parseTree cur_node = PT;
	tokenInfo lookahead = getNextToken(&d);
	while(1){ 
		int cur_sym = top(s);
		assert(cur_sym==cur_node->sym);
		if(is_terminal(cur_sym)){	
			int t_sym = find_ht(symbol_nos,lookahead.token);
			if(cur_node->sym == t_sym ){
				cur_node -> t = lookahead;
				cur_node -> is_leaf = 1;
				s = pop(s);
				lookahead = getNextToken(&d);
				cur_node = cur_node -> parent;
			}
			else if(cur_sym==0){
				cur_node -> t = epsilon_token();
				cur_node -> is_leaf = 1;
				s = pop(s);
				cur_node = cur_node -> parent;
			}
			else{
				printf("The token %s for lexeme %s does not match at line %d. The expected token here is %s\n",lookahead.token,lookahead.lxme,lookahead.line_no,symbols[cur_sym]);
				close_file();
				exit(0);
			}
		}
		if(cur_node->children_seen == 0){
			int grule = T[cur_node->sym][find_ht(symbol_nos,lookahead.token)];
			if(grule==-1){
				printf("The token %s for lexeme %s does not match at line %d. A possible expected token is %s\n",
						lookahead.token,lookahead.lxme,lookahead.line_no,symbols[first_set[cur_node->sym].rhs.head->val]);
			}
			grammar_rule r = g[grule]; 
			cur_node -> num_children = 0;
			cur_node -> gr_rule = grule;
			Node *n;
			for(n = r.rhs.head; n!=NULL; n = n->next){
				cur_node -> children[(cur_node->num_children)++] = new_tnode(n->val,cur_node);
			}
			int i;
			for(i=(cur_node->num_children)-1;i>=0;i--){
				s = push(s,cur_node -> children[i] -> sym);
			}
		}
		if(cur_node->children_seen == cur_node->num_children){
			if(cur_node->is_root){
				if(!strcmp(lookahead.token,"$")){
					//successfully completed parsing
					break;
				}
				else{
					printf("The token %s for lexeme %s does not match at line %d. The expected token here is $\n",lookahead.token,lookahead.lxme,lookahead.line_no);
					close_file();
					exit(0);
				}
			}
			else{
				s = pop(s);
				cur_node = cur_node -> parent;
			}
		}
		if( (cur_node -> children_seen) < (cur_node -> num_children) ){
			cur_node -> children_seen += 1;
			cur_node = cur_node->children[(cur_node->children_seen)-1];
		}
	}
	close_file();
	return PT;
}

void printParseTree(parseTree  PT){
	//Formatting done so that all columns are aligned
	if(PT->is_root){
		printf("%25s |L_NO | %10s | %s | %25s |LEAF?| %25s \n","LEXEME","TOKEN","VALUE(NUM/RNUM)","PARENT SYMBOL","SYMBOL");
		printf("         ---------------------------------------------------------------------------------------------------------------------\n");
	}
	if(PT -> is_leaf){
		int ival = -1;
		float rval = -1.0;
		char lexeme[25];
		printf("%25s | ",(PT->t).lxme);
		printf("%3d | ",(PT->t).line_no);
		printf("%10s | ",(PT->t).token);
		if(!strcmp(symbols[PT->sym],"NUM")){
			printf("%15d | ",(PT->t).i_val);
		}
		else if(!strcmp(symbols[PT->sym],"RNUM")){
			printf("%15.2f | ",(PT->t).r_val);
		}
		else printf("%15s | ","--------");
	}
	else{
		printf("%25s | %s | %10s | %15s | ","-----","---","-------","-------");
	}
	if(!(PT->is_root)){
		printf("%25s | ",symbols[PT->parent->sym]);
	}
	else printf("%25s | ","ROOT");
	printf(PT->is_leaf ? " yes | ":"  no | ");
	printf("%25s\n",symbols[PT->sym]);
	int i;
	for(i=0;i<(PT->num_children);i++){
		printParseTree(PT->children[i]);
	}
}
