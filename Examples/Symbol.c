#include <stdlib.h>
#include <string.h>
#include "Symbol.h"

Symbol *createVarSymbol(char *n, struct Type *t, int o) {
  Symbol *s = (Symbol *) malloc(sizeof(Symbol));
  s->name = n;
  s->flavor = VAR_SYM;;
  s->offset = o;
  s->type = t;
  s->next = NULL;
  return s;
}

Symbol *createTypeSymbol(char *n, struct Type *t) {
  Symbol *s = (Symbol *) malloc(sizeof(Symbol));
  s->name = n;
  s->flavor = TYPE_SYM;;
  s->type = t;
  s->next = NULL;
  return s;
}

SymbolTable *createSymbolTable(SymbolTable *prev) {
  SymbolTable *tbl = (SymbolTable *) malloc(sizeof(SymbolTable));
  tbl->symbols = NULL;
  tbl->prev = prev;
  tbl->next = NULL;
  return tbl;
}

void symbolInsert(SymbolTable *tbl, Symbol *s) {
  s->next = tbl->symbols;
  tbl->symbols = s;
}

Symbol *symbolLookup(SymbolTable *tbl, char *name) {
  SymbolTable *t;
  for (t = tbl; t != NULL; t = t->prev) {
    Symbol *s;
    for (s = t->symbols; s != NULL; s = s->next)
      if (strcmp(s->name, name) == 0)
	return s;
  }
  return NULL;
}
