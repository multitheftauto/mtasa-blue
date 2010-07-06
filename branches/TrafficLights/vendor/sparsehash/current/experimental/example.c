#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "libchash.h"

static void TestInsert() {
  struct HashTable* ht;
  HTItem* bck;

  ht = AllocateHashTable(1, 0);    /* value is 1 byte, 0: don't copy keys */

  HashInsert(ht, PTR_KEY(ht, "January"), 31);  /* 0: don't overwrite old val */
  bck = HashInsert(ht, PTR_KEY(ht, "February"), 28);
  bck = HashInsert(ht, PTR_KEY(ht, "March"), 31);

  bck = HashFind(ht, PTR_KEY(ht, "February"));
  assert(bck);
  assert(bck->data == 28);

  FreeHashTable(ht);
}

static void TestFindOrInsert() {
  struct HashTable* ht;
  int i;
  int iterations = 1000000;
  int range = 30;         /* random number between 1 and 30 */

  ht = AllocateHashTable(4, 0);    /* value is 4 bytes, 0: don't copy keys */

  /* We'll test how good rand() is as a random number generator */
  for (i = 0; i < iterations; ++i) {
    int key = rand() % range;
    HTItem* bck = HashFindOrInsert(ht, key, 0);     /* initialize to 0 */
    bck->data++;                   /* found one more of them */
  }

  for (i = 0; i < range; ++i) {
    HTItem* bck = HashFind(ht, i);
    if (bck) {
      printf("%3d: %d\n", bck->key, bck->data);
    } else {
      printf("%3d: 0\n", i);
    }
  }

  FreeHashTable(ht);
}

int main(int argc, char** argv) {
  TestInsert();
  TestFindOrInsert();
  return 0;
}
