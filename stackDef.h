/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: stackDef.h

********************/

struct snode;
typedef struct snode Snode;
struct snode{
	int val;
	Snode *next;
};

struct st{
	Snode *head;
	int size;
};
typedef struct st stack;
