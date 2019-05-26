#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig) {

  printf("signal %d was caught\n", sig);

  printf("Is this not a strange signal for this exception?!\n");

  /* What happens if we do not exit here? */

  exit(1);

  return;
}


int not_so_good() {
  int  x = 0;
  return 1 % x;
}


int main() {

  struct sigaction sa;

  printf("Ok, let's go - I'll catch my own error.\n");  

  sa.sa_handler = handler;
  sa.sa_flags = 0;

  /* clear the mask i.e. do not block any signals while in the handler */
  sigemptyset(&sa.sa_mask);

  /* and now we catch ... FPE signals */

  if(sigaction(SIGFPE, &sa, NULL) != 0 ) {
    fprintf(stderr, "something happend\n");
    return(1);
  }

  not_so_good();

  printf("Will probably not write this.\n");

  return(0);
}
