CC=gcc
CFLAGS=-Wall

calculator: parser.tab.o lex.yy.o main.o
	$(CC) $(CFLAGS) -o calculator parser.tab.o lex.yy.o main.o

main.o: main.c parser.h parser.tab.h
	$(CC) $(CFLAGS) -c main.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c parser.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm -f calculator *.o *.tab.c *.tab.h lex.yy.c

.PHONY: clean
