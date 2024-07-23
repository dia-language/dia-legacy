CC=gcc
MAKE=make

compiler:
	bison -d dia.y
	flex dia.l
	$(CC) -c dia.c -o dia.o
	$(CC) -c dia_calculation.c -o dia_calculation.o
	$(CC) -o diac main.c dia.o dia_calculation.o dia.tab.c lex.yy.c -lfl
	$(MAKE) clean_sources

static:
	bison -d dia.y
	flex dia.l
	$(CC) -c dia.c -o dia.o
	$(CC) -static -o diac main.c dia.o dia.tab.c lex.yy.c -lfl
	$(MAKE) clean_sources

lexer:
	flex dia.l
	$(CC) lex.yy.c -o dia_lexer -lfl

clean_sources:
	rm lex.yy.c dia.tab.c dia.tab.h *.o

clean:
	rm dia_lexer diac
