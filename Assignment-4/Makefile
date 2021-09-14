LEX=flex
YACC=win_bison
CC=g++ -std=gnu++0x
OBJECT=parser

$(OBJECT): lex.yy.o y.tab.o parser.o
	$(CC) lex.yy.o y.tab.o parser.o -o $(OBJECT)

parser.o : parser.cpp 
	$(CC) -c parser.cpp

lex.yy.o: lex.yy.c y.tab.h
	$(CC) -c lex.yy.c

y.tab.o: y.tab.c
	$(CC) -c y.tab.c

y.tab.c y.tab.h: parser_yacc.y
	$(YACC) -dy parser_yacc.y

lex.yy.c: analyser_lex.l
	$(LEX) analyser_lex.l

clean:
	@del /f $(OBJECT).exe *.o y.tab.c y.tab.h lex.yy.c