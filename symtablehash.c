/*
symtablehash.c
Author: Lisa Ochieng
Hash Table Implementation of SymTable ADT
*/

#include <stdlib.h>
#include "symtable.h"
#include <string.h>
#include <assert.h>

/* sequence of ints as bucket counts*/
static const size_t auBucketCounts[]= {
   509, 1021, 2039, 4093, 8191, 16381, 32749, 65521
      };

/* num of bucket sizes in the array*/
static const size_t uBucketCountLengths =
   sizeof(auBucketCounts)/sizeof(auBucketCounts[0]);

/* binding reps one key-value pair in a bucket's linked list*/
struct Binding{
   /*the string key used to identify the binding*/
   char *key;
   /*the value associated with the key*/
   const void *value;
   /*pointer to the next binding in the bucket's linked list*/
   struct Binding *next;
};

/*SymTable is a hashtable implemented as an array of linked lists*/
struct SymTable{
   /* array of pointers to buckets*/
   struct Binding **buckets;
   /*current num of bindings in SymTable. */
   size_t length;

   /*current indx in bucketcounts array to track table size*/
   size_t bucketCountIndex;
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   /*allocate the Symtable struct*/
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL){
      return NULL;
   }
   /*Initialize lengths and start at first int*/
   oSymTable->length = 0;
   oSymTable->bucketCountIndex = 0;

   /*allocate the array of bucket ptrs*/
   oSymTable->buckets = (struct Binding **)calloc(auBucketCounts[0],
                                             sizeof(struct Binding *));
   if (oSymTable->buckets == NULL){
      free(oSymTable);
      return NULL;
   }
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   size_t uBucket;
   struct Binding *current;
   struct Binding *next;
   size_t uCurrentBucketCount;
   assert(oSymTable != NULL);

   /* how many buckets we currently have*/
   uCurrentBucketCount = auBucketCounts[oSymTable->bucketCountIndex];
   /*loop through every bucket*/
   for (uBucket = 0; uBucket < uCurrentBucketCount; uBucket++){
      current = oSymTable->buckets[uBucket];
      /*free the linked list inside the bucket*/
      while (current != NULL){
         next = current->next;
         free(current->key); /*free duplicated string*/
         free(current); /*free the node*/
         current = next;
      }
   }
   /*free the array of bucket pointers*/
   free(oSymTable->buckets);
   /*free the main symtable structure*/
   free(oSymTable);
}
/*returns the length of oSymTable*/
size_t SymTable_getLength(SymTable_T oSymTable){
   assert(oSymTable != NULL);
   return oSymTable->length;
}
/*helper function to expand oSymTable*/
static void SymTable_expand(SymTable_T oSymTable){
   size_t uNewBucketCountIndex;
   size_t uNewBucketCount;
   struct Binding **newBuckets;
   size_t uBucket;
   struct Binding *current;
   struct Binding *next;
   size_t uNewIndex;

   /*if we are already at the max bucket count, do nothing*/
   if (oSymTable->bucketCountIndex == uBucketCountLengths - 1){
      return;
   }
   /*get new size*/
   uNewBucketCountIndex = oSymTable->bucketCountIndex + 1;
   uNewBucketCount = auBucketCounts[uNewBucketCountIndex];

   /*allocate a new array of buckets*/
   newBuckets = (struct Binding **)calloc(uNewBucketCount,
                                          sizeof(struct Binding *));
   if(newBuckets == NULL){
      /*if memorry alloc fails dont expand*/
      return;
   }
   /*rehash all existing bindings into new buckets arr*/
   for (uBucket = 0;
        uBucket < auBucketCounts[oSymTable->bucketCountIndex];
        uBucket++){
      current = oSymTable->buckets[uBucket];

      while (current != NULL){
         next = current->next; /*save pos in old bucket's chain*/
         /*calculate the new bucket index for this key*/
         uNewIndex = SymTable_hash(current->key, uNewBucketCount);
         /*Insert the node at the front of its new bucket*/
         current->next = newBuckets[uNewIndex];
         newBuckets[uNewIndex]= current;
         current = next; /*move to the next node in the old bucket*/
      }
   }
   /*Free the old buckets array and update the SymTable*/
   free(oSymTable->buckets);
   oSymTable->buckets = newBuckets;
   oSymTable->bucketCountIndex = uNewBucketCountIndex;
        }

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   size_t uBucketIndex;
   struct Binding *current;
   struct Binding *newBinding;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /*find which bucket this key belongs in*/
   uBucketIndex = SymTable_hash(pcKey,
                        auBucketCounts[oSymTable->bucketCountIndex]);
   /*check if the key already exists in bucket*/
   for(current = oSymTable->buckets[uBucketIndex];
       current!=NULL; current = current->next){
      if(strcmp(current->key, pcKey)==0){
         return 0; /*key exists*/
      }
   }

   /*key not found, allocate a new binding*/
   newBinding = (struct Binding *)malloc(sizeof(struct Binding));
   if (newBinding == NULL){
      return 0; /*insufficient memory*/
   }
   newBinding->key = (char *)malloc(strlen(pcKey)+1);
   if (newBinding->key == NULL){
      free(newBinding);
      return 0; /*insufficient memory*/
   }
   strcpy(newBinding->key, pcKey);
   newBinding->value = pvValue;

   /*insert at the front of this bucket's list*/
   newBinding->next = oSymTable->buckets[uBucketIndex];
   oSymTable->buckets[uBucketIndex] = newBinding;

   oSymTable->length++;
   /*expand hashtable if it gets full*/
   if(oSymTable->length ==
      auBucketCounts[oSymTable->bucketCountIndex]){
      SymTable_expand(oSymTable);
      }
   return 1;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   size_t uBucketIndex;
   struct Binding *current;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   uBucketIndex = SymTable_hash(pcKey,
                          auBucketCounts[oSymTable->bucketCountIndex]);
   for (current = oSymTable->buckets[uBucketIndex];
        current!=NULL; current = current->next){
      if (strcmp(current->key, pcKey)==0){
         return 1; /*key found*/
      }
   }
   return 0; /*key not found*/

}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   size_t uBucketIndex;
   struct Binding *current;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   uBucketIndex = SymTable_hash(pcKey,
                         auBucketCounts[oSymTable->bucketCountIndex]);

   for(current = oSymTable->buckets[uBucketIndex];
       current!=NULL; current = current->next){
      if (strcmp(current->key, pcKey)==0){
         return (void *)current->value;
      }
   }
   return NULL;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   size_t uBucketIndex;
   struct Binding *current;
   const void *pvOldValue;

   assert(oSymTable!=NULL);
   assert(pcKey!=NULL);

   uBucketIndex = SymTable_hash(pcKey,
                       auBucketCounts[oSymTable->bucketCountIndex]);
   for(current = oSymTable->buckets[uBucketIndex]; current!=NULL;
       current = current->next){
      if(strcmp(current->key, pcKey)==0){
         pvOldValue = current->value;
         current->value = pvValue;
         return (void *)pvOldValue;
      }
   }
   return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   size_t uBucketIndex;
   struct Binding *current;
   struct Binding *prev = NULL;
   const void *pvOldValue;

   assert(oSymTable!=NULL);
   assert(pcKey!=NULL);

   uBucketIndex = SymTable_hash(pcKey,
      auBucketCounts[oSymTable->bucketCountIndex]);
   for (current = oSymTable->buckets[uBucketIndex]; current!=NULL;
        current= current->next){
      if(strcmp(current->key, pcKey)==0){
         pvOldValue = current->value;

         /*Unlink the node from the bucket's list*/
         if (prev == NULL){
            /*the node to delete is at the front of the bucket*/
            oSymTable->buckets[uBucketIndex] = current->next;
         } else{
            /*node is in the middle or end of the bucket's chain*/
            prev->next = current->next;
         }
         /*free memory*/
         free(current->key);
         free(current);

         oSymTable->length--;
         return (void *)pvOldValue;
      }
      prev = current;
   }
   return NULL; /*key not found*/
}

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue,
     void *pvExtra),const void *pvExtra){
   size_t uBucket;
   struct Binding *current;
   size_t uCurrentBucketCount;

   assert(oSymTable!=NULL);
   assert(pfApply!= NULL);
   /*get the total num of buckets currently allocated in arr*/
   uCurrentBucketCount = auBucketCounts[oSymTable->bucketCountIndex];
   /*iterate through every single bucket index*/
   for(uBucket = 0; uBucket < uCurrentBucketCount; uBucket++){
      /*loop through linked list in this bucket*/
      for(current = oSymTable->buckets[uBucket]; current!=NULL;
          current = current->next){
         /*apply function*/
         (*pfApply)(current->key, (void *)current->value,
                    (void *)pvExtra);
      }
   }
}
