/**********************
  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: lexer.h

************************/

#include "lexerDef.h"

extern FILE *getStream(FILE *fp, buffer B, buffersize k);
extern tokenInfo getNextToken(DFA *d);
extern DFA read_dfa(const char *filename, Hashtable ht, char symbols[][25], int *num);
extern void open_file(const char *filename);
extern void close_file();
extern tokenInfo epsilon_token();
