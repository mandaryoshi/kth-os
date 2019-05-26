#include <sys/mman.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define MIN 5               //smallest block, 2⁵
#define LEVELS 8            //32, 64, 128, 256, 512, 1024, 2048, 4069
#define PAGE 4096           //largest block, 2¹²

enum flag {Free, Taken};

struct head {
  enum flag status;
  short int level;          //0 for 32, ... size of the block
  struct head *next;
  struct head *prev;
};

struct head *new() {          //allocate a full page (4Ki byte segment)
  struct head *new = (struct head *) mmap(NULL, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (new == MAP_FAILED) {
    return NULL;
  }

  assert(((long int) new & 0xfff) == 0);      //12 last bits should be 0
  new->status = Free;
  new->level = LEVELS - 1;
  return new;
}

struct head *buddy(struct head* block) {
  int index = block->level;
  long int mask = 0x1 << (index + MIN);                 //toggle 6th bit block, shift
  return (struct head*)((long int) block ^ mask);
}

struct head *merge(struct head* block, struct head* sibling) {
  struct head *primary;
  if (sibling < block) {
    primary = sibling;
  } else {
    primary = block;
  }
  primary->level = primary->level + 1;
  return primary;
}

struct head *split(struct head *block, int index) {
  int mask =  0x1 << (index + MIN);
  //block->level = block->level-1;
  return (struct head *)((long int)block | mask );
}

void *hide(struct head* block) {
  return (void*)(block + 1);
}

struct head *magic(void *memory) {
  return ((struct head*)memory -1);
}

int level (int size) {
  int req = size + sizeof(struct head);

  int i = 0;
  req = req >> MIN;
  while(req > 0) {
    i++;
    req = req >> 1;
  }
  return i;
}


void test() {
  /*struct head block;
  block = *new();
  printf("LEVEL = %d\n", block.level);
  //printf("Next = %d\n", block.level);

  struct head block2 = *split(&block, 6);
  printf("LEVEL of block2 = %d\n", block2.level);

  //var = *buddy(&block);
  printf("Buddy of block2 = %d\n", buddy(&block2)->level);

  //split = split(block*, 1000);*/

  printf("size of head is : %ld\n", sizeof(struct head));
  printf("level for 20 should be 1: %d\n", level(20));

  struct head *block = new();

  printf("LEVEL = %d\n", block->level);
  printf("Status = %ld\n", sizeof(block));

  hide(block);
  printf("Status 2 = %ld\n", sizeof(block));

  struct head *block2 = split(block, 6);

  printf("LEVEL = %ld\n", sizeof(block2));
  //printf("LEVEL = %d\n", block2->level);
}


int main(int argc, char *argv[]) {

  test();

  return 0;
}
