/*
symtable.h
Author: Lisa Ochieng
Interface for the SymTable ADT
*/
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stddef.h>

/* SymTable_T is an unordered collection of bindings. A binding has
   a string key and a void pointer value */
typedef struct SymTable *SymTable_T;

/*Returns a new SymTable object that contains no bindings, or null if
insufficient memory*/
SymTable_T SymTable_new(void);

/*Frees all memory occupied by oSymTable*/
void SymTable_free(SymTable_T oSymTable);

/*Returns the number of bindings in oSymTable*/
size_t SymTable_getLength(SymTable_T oSymTable);

/*Adds a new binding to oSymTable consisting of pcKey and pvValue. 
Returns 1 if successful, or 0 if pcKeys*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void
                 *pvValue);

/*If oSymTable contains a binding with pcKey, replaces the binding's
  value with pvValue and returns the old value. Otherwise return null*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const
                       void *pvValue);

/*Returns 1 if oSymTable contains binding with pcKey, and 0 otherwise*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*Returns the value of the binding in oSymTable whose key is pcKey,
null if no binding exists. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Removes the binding with pcKey from oSymTable and returns the
   binding's value. Returns null otherwise.*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*Applies function *pfApply ro each binding in oSymTable, passing 
pvExtra as an extra parameter*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)
                  (const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra);
#endif
