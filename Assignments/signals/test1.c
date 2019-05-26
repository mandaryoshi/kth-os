#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int volatile count;

void handler(int sig) {
  printf("signal %d was caught\n", sig);
  count++;
}

int main() {

  struct sigaction sa;

  int pid  = getpid();

  printf("ok, let's go, swend me a SIGINT by entering Ctrl-C or from another shell do kill -SIGINT %d \n", pid);

  /* this is the simple version of signal handler */
  sa.sa_handler = handler;
  sa.sa_flags = 0;

  /* clear the mask i.e. do not block any signals while in the handler */
  sigemptyset(&sa.sa_mask);

  if(sigaction(SIGINT, &sa, NULL) != 0) {
    fprintf(stderr, "something happend\n");
    return(1);
  }

  while(count != 4) {
  }

  printf("I've had enough!\n");
  return(0);
}
