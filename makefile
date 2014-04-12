all: c2html

c2html: lex.yy.c
	cc lex.yy.c -o c2html -ll

lex.yy.c: c2html.l
	lex c2html.l
clean:
	rm *.yy.c *.html c2html
