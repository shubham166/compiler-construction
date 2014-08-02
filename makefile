gcc -c linked_list.c
gcc -c hashtable.c
gcc -c stack.c
gcc -c lexer.c
gcc -c parser.c
gcc -c ast.c
gcc -c symbolTable.c
gcc -c typeExtractor.c
gcc -c semantic.c
gcc -c code_gen.c
gcc -o mycompiler driver.c linked_list.o hashtable.o stack.o lexer.o parser.o ast.o symbolTable.o typeExtractor.o semantic.o code_gen.o
