#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <math.h>
#include <errno.h>
#include "rand.h"
#include <time.h>

#define MIN  5
#define LEVELS 8
#define PAGE 4096

#define BUFFER 100
#define ROUNDS 100
#define LOOP 100

int fourkblocks = 0;

enum flag {Free, Taken};


struct head {
  enum flag status;
  short int level;
  struct head *next;
  struct head *prev;
};


/* The free lists */

struct head *flists[LEVELS] = {NULL};


int length_of_free(){
  int count = 0;
  for (int i = 0; i < LEVELS; i++) {
    struct head *block = flists[i];
    if (flists[i] != NULL) {
      while (block->next != NULL) {
        count++;
        block = block->next;
      }
    }
  }
  return count;
}

void sizes (int *buffer, int max){
  int j = 0;
  for (int i = 0; i < LEVELS; i++) {
    struct head *block = flists[i];
    if (flists[i] != NULL) {
      while (block->next != NULL & (j < max)) {
        int bytes = pow(2, i) * 32;
        buffer[i] = bytes;
        j++;
        block = block->next;
      }
    }
  }
}

/* These are the low level bit fidling operations */

struct head *new() {
  struct head *new = mmap(NULL, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(new == MAP_FAILED) {
    printf("mmap failed: error %d\n", errno);
    return NULL;
  }
  assert(((long int)new & 0xfff) == 0);  // 12 last bits should be zero
  new->status = Free;
  new->level = LEVELS -1;
  return new;
}


struct head *buddy(struct head* block) {
  int index = block->level;
  long int mask =  0x1 << (index + MIN);
  return (struct head*)((long int)block ^ mask);
}

struct head *primary(struct head* block) {
  int index = block->level;
  long int mask =  0xffffffffffffffff << (1 + index + MIN);
  return (struct head*)((long int)block & mask);
}

struct head *split(struct head *block) {
  int index = block->level - 1;
  int mask =  0x1 << (index + MIN);
  return (struct head *)((long int)block | mask );
}


void *hide(struct head* block) {
  return (void*)(block + 1);
}

struct head *magic(void *memory) {
  return (struct head*)(((struct head*)memory) - 1);
}

int level(int req) {
  int total = req  + sizeof(struct head);
  int i = 0;
  int size = 1 << MIN;
  while(total > size) {
    size <<= 1;
    i += 1;
  }
  return i;
}

void *findblock(int index){
    if (index < LEVELS) {
      if(flists[index] != NULL){
        struct head *block1 = flists[index];
        struct head *block2 = split(block1);
        //Now we have to change the variables of each new block cancer
        //block1;
        if(block1->next != NULL) {
          block1->next->prev = NULL;
          flists[index] = block1->next;
        } else {
          flists[index] = NULL;
        }
        block1->status = 0;
        block1->level = index - 1;
        block1->next = block2;
        block1->prev = NULL;
        //block2
        block2->status = 0;
        block2->level = index - 1;
        block2->next = NULL;
        block2->prev = block1;
        //Re-arrange the block;
        if(flists[index-1] != NULL){
            //add it to the end of the linked list of those blocks
        }else{
          flists[index - 1] = block1;//So it puts the first of the two blocks on the list but one level below
        }
      }else{
        findblock(index + 1); //Goes to the next block to split
      }
    }
    else{
      struct head *block = (struct head *) new();
      flists[LEVELS-1] = block;
      fourkblocks++;
    }
    return NULL;
}

struct head *find(int index) {
  if (index < LEVELS) {
    if (flists[index] != NULL) { //So if there exists a block for this index
          struct head *foundblock = flists[index];
          if (foundblock->next != NULL) {
            foundblock->next->prev = NULL; //Since the next block will be the first in the list
            flists[index] = foundblock->next;
          }else{
            flists[index] = NULL;
          }
          foundblock->status = 1;
          foundblock->next = NULL;
          foundblock->prev = NULL;
          return (struct head *)(foundblock); //REturn the address of that empty block

        //if we get hear it means that there were no avaliable blocks
    }else{
        //If we end up here that means there was no empty blocks for that index
        //We now have to go up the list and find other blocks to split

        findblock(index + 1);
        return find(index); //After splitting we run it again
      }

    return NULL;
  }
  return NULL;
}

void *balloc(size_t size) {
  if( size == 0 ){
    return NULL;
  }

  int index = level(size);
    //printf("Ballocating a block with level: %d\n", index);
  struct head *taken = find(index);
  return hide(taken);
}


void insert(struct head *block) {
  //S block is the block we wanna Free
  //First we should set it to be free
  //Then we check if this block at this level have any buddies
  //If it has, merge this block with thta block, and increase the level and add it yp index+1 LEVEL
  //return this till it cabt find a buddy anymore and leave it

  //Making it Free
  block->status = 0;
  if (block->level < LEVELS-1) {
  struct head *buddyBlock = buddy(block);
  //printf("level of buddy: %d, status: %d\n", buddyBlock->level, buddyBlock->status);

    if (buddyBlock->status == 0 && (buddyBlock->level == block->level)) {//IF there exists such a buddy

      /*printf("The block we are finding a buddy for has the status: %d\n", block->status);
      printf("The block we are finding a buddy for has the level: %d\n", block->level);
      printf("status of the buddy: %d\n", buddyBlock->status);
      printf("level of the buddy: %d\n\n", buddyBlock->level);*/

      int tempLevel = block->level;
      struct head *mergedBlock = primary(block);
      mergedBlock->level = tempLevel + 1;
      mergedBlock->status = 0;
      mergedBlock->prev = NULL;
      mergedBlock->next = NULL;
      flists[tempLevel] = NULL;
      insert(mergedBlock);
    }else{
      //Then we cant do anything other than putting it in the list
      block->next = NULL;
      block->prev = NULL;
      flists[block->level] = block;
    }
  }else{
    flists[LEVELS-1] = block;
  }
}

void bfree(void *memory) {

  if(memory != NULL) {
    struct head *block = magic(memory);
    insert(block);
  }
  return;
}

void bench3(int iterations) {

  int blocks[8] = {32,64,128,256,512,1024,2000,4000};

  for (int y = 0; y < 8; y++) {

      double total_t;
      clock_t start_balloc = clock();
      for(int i = 0; i < iterations; i++) {
        int *ballocate = balloc(blocks[y]);
        bfree(ballocate);
      }
      clock_t end_balloc = clock();
      clock_t balloc_time = (end_balloc - start_balloc);
      //fclose(fp);
      total_t = (double)(balloc_time);
      printf("Total time taken by balloc for size %d: %d\n", blocks[y], (int) total_t  );

      clock_t start_malloc = clock();
      for(int i = 0; i < iterations; i++) {
        int *mallocate = malloc(blocks[y]);
        free(mallocate);
      }
      clock_t end_malloc = clock();
      clock_t malloc_time = (end_malloc - start_malloc);
      //fclose(fp);
      total_t = (double)(malloc_time);
      printf("Total time taken by malloc for size %d: %d\n", blocks[y], (int) total_t  );
  }
}

int main(int argc, char *argv[]) {

  bench3(1000);

  return 0;
}
