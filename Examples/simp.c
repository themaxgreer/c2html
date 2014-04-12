
/*
 * Simple interpreter for programming language that consists
 * of a sequence of assignment and print statements.
 * Demo constructed in CS 355 class.
 * W. Cochran wcochran@vancouver.wsu.edu
 *
 * Grammar describing syntax
 *   (note that { } are meta-symbols indicating "zero or more")
 *   prog   -> stmts EOF
 *   stmts  -> stmt stmts
 *          ->
 *   stmt   -> IDENT '=' expr ';' 
 *          -> PRINT expr ';'
 *   expr   -> term {('+'| '-') term}
 *   term   -> factor {('*'|'/') term}
 *   factor -> '-' factor 
 *          -> '(' expr ')' 
 *          -> NUM 
 *          -> IDENT
 *
 * We parse via "recursive descent" following the grammar's lead.
 *
 * Variables are represented by IDENT and store integer values; 
 * They are not declared but pop into existance upon first reference
 * (initially assumed to hold a value of zero).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Tokens encoded as integers.
 * Single char tokens are represented by their ASCII value.
 * Other token encodings are given here. EOF is 0.
 */
#define NUM 256
#define IDENT 257
#define PRINT 258

FILE *yyin;   /* input source file being scanned by yylex() */
int yylineno; /* current line number while scanning */

/*
 * Fatal error handler.
 */
void yyerror(char *msg) {
  fprintf(stderr, "[%d]:%s\n", yylineno, msg);
  exit(-1);
}

/*
 * Type for attributes associated with tokens.
 */
union {
  int i;    /* type associated with NUM */
  char *s;  /* type associated with IDENT */
} yylval;

/*
 * Lexical Analyzer.
 * Returns next token.
 * Corresponding attributes are stored in global yylval.
 */
int yylex(void) {
  int c;
 top:
  while ((c = fgetc(yyin)) != EOF && (c == ' ' || c == '\t' || c == '\r'))
    ;
  if (c == '\n') {
    yylineno++;
    goto top;
  }
  if (c == EOF) return 0;
  if (strchr("+-();*=", c) != NULL)
    return c;
  if (c == '/') {
    if ((c = fgetc(yyin)) == '/') {
      while ((c = fgetc(yyin)) != '\n' && c != EOF)
	;
      if (c == EOF) return 0;
      yylineno++;
      goto top;
    }
    ungetc(c, yyin);
    return '/';
  }
#define DIGIT(c) ('0' <= (c) && (c) <= '9')
  if (DIGIT(c)) {
    int n = 0;
    do {
      n = n*10 + c - '0';
      c = fgetc(yyin);
    } while (DIGIT(c));
    ungetc(c, yyin);
    yylval.i = n;
    return NUM;
  }
#define ALPHA(c) (('a' <= (c) && (c) <= 'z') || ('A' <= (c) && (c) <= 'Z'))
  if (ALPHA(c) || c == '_') {
    char buf[100];
    int i = 0;
    do {
      buf[i++] = c;
      c = fgetc(yyin);
    } while (ALPHA(c) || DIGIT(c) || c == '_');
    buf[i++] = '\0';
    ungetc(c, yyin);
    if (strcmp("print", buf) == 0)
      return PRINT;
    yylval.s = strdup(buf);
    return IDENT;
  }
  yyerror("unknown lexeme!");
  return 0;
}

int tok; /* lookahead token for LL(1) recursive descent parse */

void match(int t) {
  if (t != tok)
    yyerror("unexpected token!\n");
  tok = yylex();
}

void stmts(void);
void stmt(void);
int expr(void);
int term(void);
int factor(void);

/*
 * Top level function that drives recursive descent parser.
 * Corresponds to grammar start symbol.
 *   prog   -> stmts EOF
 */
void prog(void) {
  stmts();
  match('\0');
}

/*
 *   stmts  -> stmt stmts
 *          ->
 */
void stmts(void) {
  if (tok == IDENT || tok == PRINT) {
    stmt();
    stmts();
  }
}

/*
 * Type for node in symbol table (a simple linked-list).
 */
typedef struct Symbol {
  char *name;  /* name of symbol */
  int val;     /* current value of corresponding variable */
  struct Symbol *next;
} Symbol;

/*
 * Symbol Table.
 */
Symbol *symtab = NULL;

/*
 * Returns node corresponding to name in Symbol Table.
 * Returns NULL of name not in table.
 */
Symbol *lookup(char *name) {
  Symbol *n;
  for (n = symtab; n != NULL; n = n->next)
    if (strcmp(name, n->name) == 0)
      return n;
  return NULL;
}

/*
 * Inserts node for name in Symbol Table.
 * If symbol already in table, that val is just updates.
 */
Symbol *insert(char *name, int val) {
  Symbol *s = lookup(name);
  if (s == NULL) {
    s = (Symbol *) malloc(sizeof(Symbol));
    s->name = name;
    s->next = symtab;
    symtab = s;
  }
  s->val = val;
  return s;
}

/*
 *   stmt   -> IDENT '=' expr ';' 
 *          -> PRINT expr ';'
 */
void stmt(void) {
  if (tok == IDENT) {
    char *name = yylval.s;
    int val;
    match(IDENT);
    match('=');
    val = expr();
    match(';');
    insert(name, val);
  } else if (tok == PRINT) {
    int val;
    match(PRINT);
    val = expr();
    match(';');
    printf("%d\n", val);
  } else {
    yyerror("unknown statement");
  }
}

/*
 *   expr   -> term {('+'| '-') term}
 */
int expr(void) {
  int v = term();
  while (tok == '+' || tok == '-') {
    int u, t = tok;
    match(tok);
    u = term();
    if (t == '+') v += u;
    else v -= u;
  }
  return v;
}

/*
 *   term   -> factor {('*'|'/') term}
 */
int term(void) {
  int v = factor();
  while (tok == '*' || tok == '/') {
    int u, t = tok;
    match(tok);
    u = factor();
    if (t == '*') v *= u;
    else v /= u;
  }
  return v;
}

/*
 *   factor -> '-' factor 
 *          -> '(' expr ')' 
 *          -> NUM 
 *          -> IDENT
 */
int factor(void) {
  int v;
  switch(tok) {
  case '-': match('-'); return -factor();
  case '(': match('('); v = expr(); match(')'); return v;
  case NUM: v = yylval.i; match(NUM); return v;
  case IDENT: {
    Symbol *s = lookup(yylval.s);
    if (s == NULL) s = insert(yylval.s, 0);
    v = s->val;
    match(IDENT);
    return v;
  }
  default: yyerror("unknown factor");
  }
  return 0;
}

int main(void) {
  yyin = stdin;
  yylineno = 1;
  tok = yylex();
  prog();
  return 0;
}