/**********

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: lexerDef.h

**********/

#include "hashtable.h"

#define BUF_SIZE 500
#define MAX_STATES 200

typedef unsigned int buffersize;
typedef char buffer[BUF_SIZE];


struct token{
	int i_val;
	float r_val;
	char lxme[30];
	char token[30];
	int line_no;
};
typedef struct token tokenInfo;

struct dfa{
	int num_states;
	int start_state;
	int transitions[MAX_STATES][128]; 
	int is_final[MAX_STATES];
	int to_backtrack[MAX_STATES];
	char tok[MAX_STATES][25];  
};

typedef struct dfa DFA;
