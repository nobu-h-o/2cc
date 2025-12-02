CC=gcc
CFLAGS=-Wall
TARGET=2cc

2cc: parser.tab.o lex.yy.o main.o codegen.o ast.o symtab.o
	$(CC) $(CFLAGS) -o 2cc parser.tab.o lex.yy.o main.o codegen.o ast.o symtab.o

main.o: main.c parser.h parser.tab.h codegen.h ast.h symtab.h
	$(CC) $(CFLAGS) -c main.c

codegen.o: codegen.c codegen.h ast.h symtab.h
	$(CC) $(CFLAGS) -c codegen.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c parser.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

test: $(TARGET)
	sh ./test.sh

clean:
	rm -f 2cc tmp *.o *.tab.c *.tab.h lex.yy.c *.s

.PHONY: clean
