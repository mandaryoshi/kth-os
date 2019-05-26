#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "rand.h"

#include "buddy.h"

#define BUFFER 100
#define SIZES 20000

#define ROUNDS 100
#define LOOP 100

int freq[SIZES];


int length_of_free();

void sizes(int *buffer, int max);

int cmp(const void *a, const void *b) {
  return *(int*)a - *(int*)b;
}

int main(int argc, char *argv[]) {

  if(argc < 3) {
    printf("usage: bench <rounds> <loop> <file>\n");
    exit(1);
  }

  int rounds = atoi(argv[1]);
  int loop = atoi(argv[2]);
  char *name = argv[3];

  void *init = sbrk(0);
  void *current;

  void *buffer[BUFFER];
  for(int i =0; i < BUFFER; i++) {
    buffer[i] = NULL;
  }

  printf("The initial top of the heap is %p.\n", init);

  for(int j = 0; j < rounds; j++) {
    for(int i= 0; i < loop ; i++) {
      int index = rand() % BUFFER;
      if(buffer[index] != NULL) {
	       bfree(buffer[index]);
      }
      size_t size = (size_t)request();
      int *memory;
      memory = balloc(size);

      if(memory == NULL) {
	       //memory = balloc(0);
	       fprintf(stderr, "memory myllocation failed, last address %p\n", memory);
	       return(1);
      }
      buffer[index] = memory;
      /* writing to the memory so we know it exists */
      *memory = 123;
    }
    current = sbrk(0);
    int allocated = (int)((current - init) / (1024));
    printf("The final top of the heap is %p.\n", current);
    printf("   increased by %d Ki byte\n", allocated);
    //printf("   length of free list is %d\n", length_of_free());
  }

  FILE *file = fopen(name, "w");

  sizes(freq, SIZES);
  //int length = length_of_free();

  qsort(freq, 20000, sizeof(int), cmp);

  for(int i = 0; i < 20000; i++) {
    fprintf(file, "%d\n", freq[i]);
  }

  fclose(file);

  printf("<return> to continue\n");
  char ch;
  read(STDIN_FILENO, &ch, 1);

  return 0;
}
