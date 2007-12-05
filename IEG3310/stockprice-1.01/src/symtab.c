/*******************************************************************************
 *
 * symtab.c
 * CSC2101 Data Struture
 * Assignment 2
 *
 * Guan Hao
 * 05569511
 * raptium[AT]gmail.com
 *
 * 3 Feb, 2007
 *
 * Implementation for symtabADT.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "symtab.h"
#include "string.h"
#define MULTIPLIER -16644117991L

typedef struct entryT {
    char *key;
    void *value;
}

entryT;

struct symtabCDT {
    listADT buckets[101];
};

// Addtional functions declaration.
listADT DeleteEntry(listADT list, char *key);
entryT *FindEntry(listADT list, char *key);
void ForEachEntryInListDo(symtabFnT fp, listADT list);

// Hash function for the symbol table.
int Hash(char *s, int nBuckets) {
    int i;
    unsigned long hashCode;
    hashCode = 0;

    for (i = 0; s[i] != '\0'; i++)
        hashCode = hashCode * MULTIPLIER + s[i];

    return (hashCode % nBuckets);
}

// Create an empty symbol table.
symtabADT EmptySymbolTable() {
    symtabADT table;

    table = (symtabADT)malloc(sizeof(struct symtabCDT));

    memset(table, 0, sizeof(struct symtabCDT));
    return table;
}

// Enter entry to a symbol table form key and value.
void Enter(symtabADT table, char *key, void *value) {
    entryT *entry;
    listADT list;
    int hashcode;
    hashcode = Hash(key, 101);
    list = table->buckets[hashcode];
    entry = FindEntry(list, key); // Find the entry with the key input.

    if (entry != NULL)
        entry->value = value; // If an entry with the key input exists, update its value.
    else {
        entry = (entryT *)malloc(sizeof(entryT)); // Create a new entry
        entry->key = key;
        entry->value = value;
        table->buckets[hashcode] = ListCons(entry, list); // Update the list with new entry.
    }
}

// Delete an entry with the key input.
void Delete(symtabADT table, char *key) {
    listADT list;
    list = table->buckets[Hash(key,101)];

    if (!ListIsEmpty(list))
        table->buckets[Hash(key,101)] = DeleteEntry(list, key);
}

// Try to find the entry with the key input.
void *Lookup(symtabADT table, char *key) {
    listADT list;
    entryT *entry;
    list = table->buckets[Hash(key,101)];
    entry = FindEntry(list, key);

    if (entry == NULL)
        return NULL; // Return NULL if the entry can not be find in the table.

    return entry->value; // Return the value of the entry
}

// Call an fp function for each entry.
void ForEachEntryDo(symtabFnT fp, symtabADT table) {
    int i;
    listADT list;

    for (i = 0;i < 101;i++) {
        list = table->buckets[i];

        if (!ListIsEmpty(list))
            ForEachEntryInListDo(fp, list); // Call recursion helper
    }
}

// Recursion helper for ForEachEntryDo function.
void ForEachEntryInListDo(symtabFnT fp, listADT list) {
    entryT *entry;

    if (!ListIsEmpty(list)) {
        entry = ListHead(list);
        fp(entry->key, entry->value);
        ForEachEntryInListDo(fp, ListTail(list));
    }
}

// Recursion helper for Delete function, it deletes entry in list.
listADT DeleteEntry(listADT list, char *key) {
    entryT *entry;
    listADT newlist;

    if (ListIsEmpty(list))
        return EmptyList();

    entry = ListHead(list);

    if (!strcmp(entry->key, key)) {
        newlist = ListTail(list);
        free(list);
        return newlist;
    } else {
        return ListCons(ListHead(list), DeleteEntry(ListTail(list), key));
    }
}

// Recursion helper, it returns entry with certain key in list.
entryT *FindEntry(listADT list, char *key) {
    entryT *entry;

    if (ListIsEmpty(list))
        return NULL;

    entry = ListHead(list);

    if (strcmp(entry->key, key))
        return FindEntry(ListTail(list), key);

    return entry;
}
