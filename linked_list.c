/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: linked_list.c

********************/

#include <stdio.h>
#include <malloc.h>
#include "linked_list_def.h"

linked_list new_LL(){
	linked_list l;
	l.head = NULL;
	l.size = 0;
	return l;
}

linked_list insert_ll(linked_list l, int val){
	Node *n  = (Node *) malloc(sizeof(Node));
	n -> val = val;
	n -> next = NULL;
	l.size++;
	if(l.size==1){
		l.head = n;
	}
	else{
		Node *cur = l.head;
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur -> next = n;
	}
	return l;
}

int find_ll(linked_list l, int val){
	Node *cur = l.head;
	while(cur!=NULL){
		if(cur->val == val){
			return 1;
		}
		cur = cur->next;
	}
	return 0;
}
