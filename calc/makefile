calc: calc.l calc.y
	bison -d calc.y
	flex calc.l
	gcc -o calc calc.tab.c lex.yy.c -ll

clean:
	rm -f calc calc.tab.c calc.tab.h lex.yy.c

test: calc
	./calc
