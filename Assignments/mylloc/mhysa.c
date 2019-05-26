#include <stdlib.h>
#include <unistd.h>

struct chunk {
  int size;
  struct chunk *next;
};


struct chunk *flist = NULL;

void *malloc(size_t size) {
  if( size == 0 ){
    return NULL;
  }

  struct chunk *next = flist;
  struct chunk *prev = NULL;
  // printf("check free list %p for size %d\n", flist, (int)size);

  while(next != NULL) {
    if (next->size >= size) {
      // printf("block found %p\n", next);
      if(prev != NULL) {
	       prev->next = next->next;
      } else {
	       flist = next->next;
      }
      return  (void*)(next + 1);
    } else {
      prev = next;
      next = next->next;
    }
  }
  // printf("allocating new memory\n");
  /* use sbrk to allocate new memory */
  void *memory = sbrk(size + sizeof(struct chunk));
  if(memory == (void *)-1) {
    return NULL;
  } else {
    struct chunk *cnk = (struct chunk*)memory;
    cnk->size = size;
    return  (void*)(cnk + 1);
  }

}

void free(void *memory) {

  if(memory != NULL) {
    /* we're jumping back one chunk position */
    struct chunk *cnk = (struct chunk*)((struct chunk*)memory - 1);
    cnk->next = flist;
    // printf("free chunk %p of size %d\n", cnk, cnk->size);
    flist = cnk;
  }
  return;
}
