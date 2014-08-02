/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: hashtable.c

********************/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "hashDef.h"

int hash(char *s){
	int l = strlen(s),i,h = 0;
	for(i=0;i<l;i++){
		h = h*137 + s[i];
	}
	return h;
}

Hashtable new_HT(int capacity){
	Hashtable h;
	h.size = capacity;
	h.array = (Entry *) malloc(capacity*sizeof(Entry));
	int i;
	for(i=0;i<capacity;i++)
		h.array[i] = NULL;
	return h;
}

void insert_ht(Hashtable h, char *s, int value){
	int pos = hash(s) % h.size, j = 0;
	while(pos<0)
		pos += h.size;
	while(h.array[pos]!=NULL){
		j++;
		pos = (pos + j*j)%h.size;
	}
	h.array[pos] = (Entry) malloc(sizeof(struct entry));
	strcpy(h.array[pos]->str,s);
	h.array[pos]->val = value;
}

int find_ht(Hashtable h, char *s){
	int pos = hash(s) % h.size, j = 0;
	while(pos<0)
		pos += h.size;
	while(h.array[pos]!=NULL && strcmp(h.array[pos]->str,s) ){
		j++;
		pos = (pos + j*j) % h.size;
	}
	if(h.array[pos]==NULL){
		// not found
		return -1;
	}
	return h.array[pos]->val;
}
