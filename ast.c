/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: ast.c

********************/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "astDef.h"

ast_rules asr;
void read_ast_rules(){
	asr = (ast_rules) malloc(NUM_RULES*sizeof(ast_rule));
	int i;
	FILE *fp = fopen("astrules.txt","r");
	char s[300];
	for(i=0;i<num_rules;i++){
		fgets(s,300,fp);
		asr[i].rhs = new_LL();
		int bs=0,b,num;
		while(sscanf(s+bs,"%d%n",&num,&b)>0){
			asr[i].rhs = insert_ll(asr[i].rhs,num);
			bs += b;
		}
		Node *it = asr[i].rhs.head;
	}
	strcpy(symbols[200],"Mat_Elem");
}

void create_ast(parseTree pt){
	fflush(stdout);
	if(pt -> is_leaf){
		pt -> discard = 0;
		return;
	}
	int rul = pt -> gr_rule;
	//format of rule: LHS -> corresponding grammar rule no.
	//RHS -> keep_node? (1/0) <sp> child whose sym to make nod (-1 if own) <sp> merge/child (1/0) <sp> child nos for children / merge
	linked_list l = asr[rul].rhs;
	Node *temp = l.head;
	int nod = temp -> val;
	temp = temp -> next;
	int s = temp -> val;
	temp = temp -> next;
	int merge = (temp -> val);
	temp = temp -> next;
	if(pt -> gr_rule==46){
		if(pt->children[1]->gr_rule==48)
			s = -1, nod = 0, merge = 1;
	}
	if(pt->gr_rule==49){
		if(pt->children[1]->gr_rule==51)
			s = -1, nod = 0, merge = 1;
	}
	if(pt->gr_rule==13){
		if(pt->parent->gr_rule != 12)
			nod = 0;
	}
	if(pt->gr_rule==64 && pt->children[1]->gr_rule!=76){
		nod = 1;
		merge = 0;
		strcpy(pt -> t.token, "MAT_ELEM");
		pt -> sym = 200;
	}
	if(nod){
		pt -> discard = 0;
	}
	if(s != -1){
		create_ast( pt -> children[s]);
		pt -> sym = pt -> children[s] -> sym;
		pt -> t = pt -> children[s] -> t;
	}
	int i, num_ast_ch = 0;
	parseTree ast_children[30];
	for(i=0; i< (pt->num_children); i++){
		pt -> children[i] -> discard = 1;
	}
	Node *it;
	for(it = temp; it != NULL; it = it->next){ 
		parseTree ch = pt -> children[it -> val];
		if(ch -> gr_rule == 35){
			ast_children[num_ast_ch++] = ch;
		}
		if(it->val != s)
			create_ast(ch); // recursion
		if(merge){
			pt -> sym = ch -> sym;
			pt -> t = ch -> t;
			pt -> discard = ch -> discard;
			for(i=0; i<ch->num_children; i++){
				ast_children[num_ast_ch++] = ch -> children[i];
			}
			break;
		} else{
			if(ch -> discard)
				for(i=0; i<ch->num_children; i++){
					ast_children[num_ast_ch++] = ch -> children[i];
				}
			else{
				ast_children[num_ast_ch++] = ch;
			}
		}
	}
	for(i=0;i< (pt->num_children); i++){
		if( pt -> children[i] -> discard ){
			free(pt->children[i]);
		}
	}
	//replace children of pt with ast_children
	pt -> num_children = num_ast_ch;
	for(i=0; i<num_ast_ch; i++){
		pt -> children[i] = ast_children[i];
		pt -> children[i] -> parent = pt;
	}
	if(pt -> num_children == 0){
		pt -> is_leaf = 1;
	}
	else
		pt -> is_leaf = 0;
}
