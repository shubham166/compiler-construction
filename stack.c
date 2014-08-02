/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: stack.c

********************/

#include "stackDef.h"
#include <malloc.h>

stack new_st(){
	stack s;
	s.head = NULL;
	s.size = 0;
	return s;
}

stack push(stack s, int elem){
	Snode *n = (Snode *) malloc(sizeof(Snode));
	n -> val = elem;
	n -> next = s.head;
	s.head = n;
	s.size++;
	return s;
}

int is_empty(stack s){
	return s.size==0;
}

int top(stack s){
	return s.head -> val;
}

stack pop(stack s){
	Snode *temp = s.head;
	s.head = temp -> next;
	free(temp);
	s.size--;
	return s;
}
