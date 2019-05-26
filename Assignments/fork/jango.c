#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {

  //int fd = open("quotes.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  int pid = fork();

  if(pid == 0) {

    int fd = open("quotes.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    printf("new entry %d \n", fd);

    dup2(fd, 1);
    close(fd);
    execl("boba", "boba", NULL);
  } else {
    //dprintf(fd, "Arghhhh!");
    //sleep(2);
    //dprintf(fd, "OK!");
    wait(NULL);
  }
  return 0;
}
