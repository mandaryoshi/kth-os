#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PAGES (16)
#define REFS (1024 * 1024)
#define PAGESIZE (4*1024)

int main(int argc, char const *argv[]) {
  clock_t c_start, c_stop;

  char *memory = malloc((long) PAGESIZE * PAGES);

  for (int p = 0; p < PAGES; p++) {
    long ref = (long) p * PAGESIZE;
    memory[ref] += 1;
  }

  printf("#TLB experiment\n");
  printf("#  page size = %d bytes\n", (PAGESIZE));
  printf("#  max number of pages = %d\n",  PAGES);
  printf("#  total number or references = %d Mi\n", (REFS/(1024*1024)));
  //printf("#  time for all references in sec \n"),
  //printf("#pages\t proc\t wall\t sum\n");
  printf("#pages\t proc\t sum\n");

  for (int pages = 1; pages < PAGES; pages+=1) {

    int loops = REFS/pages;

    c_start = clock();

    long sum = 0;

    for (int l = 0; l < loops; l++) {
      for (int p = 0; p < pages; p++) {
        long ref = (long) p * PAGESIZE;
        sum += memory[ref];
        //sum++;
      }
    }

    c_stop = clock();

    {
      double proc;

      proc = ((double)(c_stop - c_start))/CLOCKS_PER_SEC;
      printf("%d\t %.6f\t %ld\n", pages, proc, sum);
    }

  }

  return 0;
}
