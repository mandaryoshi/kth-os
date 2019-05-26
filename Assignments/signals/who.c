#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile int done;


void handler(int sig, siginfo_t *siginfo, void *cntx) {

  printf("signal %d was caught\n", sig);

  printf("your UID is %d\n", siginfo->si_uid);
  printf("your PID is %d\n", siginfo->si_pid);

  /* The third argument, cntx, is the context in which the signal was
     generated. If you mess with this you know what you're doing! */

  done = 1;
}




int main() {

  struct sigaction sa;

  int pid = getpid();

  printf("Ok, let's go - kill %d and I'll tell you who you are.\n", pid);

  /* we're using the more elaborative sigaction handler */
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  /* clear the mask i.e. do not block any signals while in the handler */
  sigemptyset(&sa.sa_mask);

  if(sigaction(SIGINT, &sa, NULL) != 0) {
    fprintf(stderr, "something happend\n");
    return(1);
  }

  while(!done) {
  }

  printf("Told you so!\n");
  return(0);
}
