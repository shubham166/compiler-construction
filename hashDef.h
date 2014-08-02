/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: hashDef.h

********************/


struct entry{
	char str[25];
	int val;
};

typedef struct entry *Entry;

struct ht{
	int size;
	Entry *array;
};

typedef struct ht Hashtable;


