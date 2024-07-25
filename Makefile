CC=gcc
MAKE=make

compiler: dia_calculation.o dia_vector.o dia.o
	$(CC) -o diac main.c dia.o dia_calculation.o dia_vector.o dia.tab.c lex.yy.c -lfl
	$(MAKE) clean_sources

static: dia_calculation.o dia_vector.o dia.o
	$(CC) -static diac main.c dia.o dia_calculation.o dia_vector.o dia.tab.c lex.yy.c -lfl
	$(MAKE) clean_sources

bison_flex:
	bison -d dia.y
	flex dia.l

dia_calculation.o: bison_flex
	$(CC) -c dia_calculation.c -o dia_calculation.o

dia_vector.o: bison_flex
	$(CC) -c dia_vector.c -o dia_vector.o

dia.o: bison_flex
	$(CC) -c dia.c -o dia.o


lexer:
	flex dia.l
	$(CC) lex.yy.c -o dia_lexer -lfl

clean_sources:
	rm lex.yy.c dia.tab.c dia.tab.h *.o

clean:
	rm dia_lexer diac
