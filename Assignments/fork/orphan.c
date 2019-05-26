#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

  int pid = fork();

  if(pid == 0) {
    int child = getpid();

    printf("child(%d): parent %d, group %d, session %d\n", getpid(),  getppid(), getpgid(child), getsid(child));
    sleep(10);
    printf("child(%d): parent %d, group %d, session %d\n", getpid(), getppid(), getpgid(child), getsid(child));
    sleep(10);
    printf("child(%d): parent %d, group %d, session %d\n", getpid(), getppid(), getpgid(child), getsid(child));
  } else {
    int parent = getpid();
    printf("parent(%d): parent %d, group %d, session %d\n", getpid(), getppid(), getpgid(parent), getsid(parent));
    sleep(5);
    int zero = 0;
    int i = 3 / zero;
  }
  return 0;
}
