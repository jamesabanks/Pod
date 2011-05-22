
all:		cff

.PHONY: clean
clean:
	rm -f *.o lex.yy.c cff

lex.yy.c:   cff.l
	flex cff.l

text_functions.o:	text_functions.c

cff:		lex.yy.c text_functions.o
	cc lex.yy.c text_functions.o -o cff -lfl
