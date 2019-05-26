#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>


#include "joshua.h"

int main(int argc, char *argv[])
{
  char *file_name = "/dev/joshua";
  int fd;

  fd = open(file_name, O_RDONLY);

  if (fd == -1) {
    perror("Joshua is not available");
    return 2;
  }

  char buffer[JOSHUA_MAX];

  if (ioctl(fd, JOSHUA_GET_QUOTE, &buffer) == -1) {
    perror("Hmm, not so good");
  } else {
    printf("Quote - buffer at 0x%p\n", buffer);
    printf("Quote - %s\n", buffer);
  }

  close (fd);
  return 0;
}
