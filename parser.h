/******************

  BATCH NO 15
  Akhilesh Anandh - 2011A7PS030P
  Shubham Agarwal - 2011A7PS124P

	file: parser.h

********************/

#include "parserDef.h"
extern void read_grammar();
extern void createParseTable(grammar G, table T);
extern parseTree parseInputSourceCode(char *testCaseFile, table T);
extern void printParseTree(parseTree PT);
