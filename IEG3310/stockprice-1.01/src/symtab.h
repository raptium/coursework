#ifndef SYMTAB_H_
#define SYMTAB_H_
typedef struct symtabCDT *symtabADT;

typedef void (*symtabFnT)(char *, void *);

symtabADT EmptySymbolTable();
void Enter(symtabADT table, char *key, void *value);
void Delete(symtabADT table, char *key);
void *Lookup(symtabADT table, char *key);
void ForEachEntryDo(symtabFnT fp, symtabADT table);

#endif /*SYMTAB_H_*/
