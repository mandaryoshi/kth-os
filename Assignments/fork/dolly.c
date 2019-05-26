/*#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

  int pid = fork();

  if(pid == 0) {
    printf("I'm the child process and my pid is %d\n", getpid());
    sleep(1);
    return 42;
  } else {
    printf("I'm the mother process and my child is called %d\n", pid);
    int res;
    wait(&res);
    printf("My child has terminated\n");
    printf("the result was %d\n", WEXITSTATUS(res));
  }
  printf("This is the end (%d)\n", getpid());

  return 0;
}
*/

/*#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

  int pid;

  int x = 123;

  pid = fork();

  if(pid == 0) {
    printf("  child:  x is  %d  (0x%p)\n", x, &x);
    x = 42;
    sleep(1);
    printf("  child:  x is %d  (0x%p)\n", x, &x);
  } else {
    printf("  mother: x is  %d  (0x%p)\n", x, &x);
    x = 13;
    sleep(1);
    printf("  mother: x is %d  (0x%p)\n", x, &x);
    wait(NULL);
  }

  return 0;
}*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

  int pid = fork();

 if (pid == 0) {
   int child = getpid();
   printf("I'm the child %d in group %d\n", child, getpgid(child));
 }
 else {
   int parent = getpid();
   printf("I'm the parent %d in group %d\n", parent, getpgid(parent));
   wait(NULL);
 }
 return 0;
}
