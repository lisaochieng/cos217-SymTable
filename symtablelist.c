/*
symtablelist.c
Author: Lisa Ochieng
Linked list implementation of SymTable ADT
*/
#include "symtable.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*Node represents a single binding in SymTable*/
struct Node {
   /*string key that uniquely identifies the binding*/
   char *key;
   /*value associated with key, not owned by SymTable*/
   const void *value;
   /*pointer to next node in linked list*/
   struct Node *next;
};

/*SymTable is an unordered collection of bindings implemented
as a linked list*/
struct SymTable {
   /*pointer to first node*/
   struct Node *head;
   /*current number of bindings in the SymTable*/
   size_t length;
};

/*Returns a new SymTable object that has no bindings, null if 
insufficient memory*/
SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL){
      return NULL;
   }

   oSymTable->head = NULL;
   oSymTable->length = 0;

   return oSymTable;
}

/* frees all memory occupied by oSymTable*/
void SymTable_free(SymTable_T oSymTable){
   struct Node *current;
   struct Node *next;

   if (oSymTable == NULL){
      return;
   }

   current = oSymTable->head;
      /*traverse list to free each node and its owned str key*/
      while (current!=NULL){
         next = current->next;

         /*free key because table owns it*/
         free(current->key);
         /*free the node*/
         free(current);

         current = next;
      }
   /*free table*/
   free(oSymTable);
}

/*returns number of bindings in oSymTable*/
size_t SymTable_getLength(SymTable_T oSymTable){
   assert(oSymTable != NULL);
   return oSymTable->length;
}

/*adds a new binding to oSymTable consisting of pcKey and pvValue.
return 1 if successful, 0 if pcKey is in table already or if 
insufficient memory available*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   struct Node *current;
   struct Node *newNode;
   assert (oSymTable != NULL);
   assert(pcKey != NULL);

   /*linear search to check if key exists*/
   for(current = oSymTable->head; current!=NULL;
       current = current->next){
      if(strcmp(current->key, pcKey) == 0){
         return 0; /*key exists*/
      }
   }

   /*allocate memory for new node*/
   newNode = (struct Node *)malloc(sizeof(struct Node));
   if (newNode == NULL){
      return 0; /*insufficient memory*/
   }

   /*make a copy of key string including space for null terminator*/
   newNode->key = (char *)malloc(strlen(pcKey)+1);
   if(newNode->key == NULL){
      free(newNode);
      return 0; /*insufficient memory*/
   }
   strcpy(newNode->key, pcKey);
   /*store the pointer to the value*/
   newNode->value = pvValue;

   /*insert the new node at the head of the list so its faster*/
   newNode->next = oSymTable->head;
   oSymTable->head = newNode;
   oSymTable->length++;

   return 1;
   
}

/*if oSymTable contains binding with pcKey, replaces the binding's val
with pvValue and returns the old val, else returns NULL*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct Node *current;
   const void *oldValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /*seach for key and update its value if found*/
   for(current = oSymTable->head; current!=NULL;
       current = current->next){
      if (strcmp(current->key, pcKey)==0){/*found*/
         oldValue = current->value;
         current->value = pvValue;
         /*cast to much return type*/
         return (void *)oldValue;
      }
   }
   return NULL;
}

/*return 1 if oSymTable contains a binding with pcKey, 0 otherwise*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct Node *current;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /*linear search for match*/
   for (current = oSymTable->head; current!=NULL;
        current=current->next){
      if(strcmp(current->key, pcKey)==0){/*match found*/
         return 1;
      }
   }

   return 0;
}

/*returns the val of the binding in oSymtable whose key is pcKey, else
NULL*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Node *current;
   assert(oSymTable!=NULL);
   assert(pcKey!=NULL);

   /*linear search*/
   for (current = oSymTable->head; current!=NULL;
        current=current->next){
      if(strcmp(current->key, pcKey) == 0){
         /*match found*/
         return (void *)current->value;
      }
   }
   return NULL;
}

/*removes the binding with pcKey from oSymtable and returns the 
binding's value, else return NULL*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct Node *current;
   struct Node *prev = NULL;
   const void *oldValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /*linear search list while keeping track of prev*/
   for(current = oSymTable->head; current!=NULL;
       current = current->next){
      if(strcmp(current->key, pcKey)==0){
         /*match found so unlink current from list*/
         if(prev == NULL){
            /*target node is at head*/
            oSymTable->head = current->next;
         } else{
            /* target is at middle or end*/
            prev->next = current->next;
         }
         oldValue = current->value;

         /* free the copy of string key*/
         free(current->key);
         /*free the node*/
         free(current);
         oSymTable->length--;

         return (void *)oldValue;
      }
      prev = current;
   }
   return NULL;
}

/*applies function *pfApply to each binding in oSymTable, passing 
pvExtra as an extra parameter*/
void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra){
   struct Node *current;
   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   /*search through list and apply fn to each binding*/
   for (current = oSymTable->head; current !=NULL;
        current= current->next){
      (*pfApply)(current->key, (void *)current->value,
                 (void *)pvExtra);
   }
}

   
