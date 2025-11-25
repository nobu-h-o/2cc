CC=gcc
CFLAGS=-Wall
TARGET=2cc

2cc: parser.tab.o lex.yy.o main.o codegen.o ast.o
	$(CC) $(CFLAGS) -o 2cc parser.tab.o lex.yy.o main.o codegen.o ast.o

main.o: main.c parser.h parser.tab.h codegen.h ast.h
	$(CC) $(CFLAGS) -c main.c

codegen.o: codegen.c codegen.h ast.h
	$(CC) $(CFLAGS) -c codegen.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

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
	rm -f 2cc tmp *.o *.tab.c *.tab.h lex.yy.c codegen.o ast.o *.s

.PHONY: clean
