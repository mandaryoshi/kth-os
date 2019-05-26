#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define ITERATIONS 10
#define BURST 10


int main() {
  int descr[2];

  assert(0 == pipe(descr));

  if(fork() == 0 ) {
    /* child process */
    for(int i = 0; i < ITERATIONS; i++) {
      for(int j = 0; j < BURST; j++) {
	int buffer[10];
	read(descr[0], &buffer, 10);
      }
      sleep(1);
    }
    printf("consumer done\n");
    return 0;
  }

  /* produce quickly */
  for(int i = 0; i < ITERATIONS; i++) {
    for(int j = 0; j < BURST; j++) {
      write(descr[1], "0123456789", 10);
    }
    printf("producer burst %d done\n", i);
  }
  printf("producer done\n");

  wait(NULL);
  printf("all done\n");
}
