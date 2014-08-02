/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: linked_list_def.h

********************/



struct node;
typedef struct node Node;

struct node{
	int val;
	Node *next;
};

struct ll{
	int size;
	Node *head;
};

typedef struct ll linked_list;
