#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

  int pid = fork();

  if(pid == 0) {
    printf("check the status using ps -ao pid,stat,command\n");
    sleep(10);
    printf("and again\n");
    return (int)42;
  } else {
    sleep(20);
    int res;
    wait(&res);
    printf("the result was %d\n", WEXITSTATUS(res));
    sleep(10);
    printf("and again\n");
  }
  return 0;
}
