#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "buddy.h"

void test() {
  block = new();
  split(block, 1000);
}


int main(int argc, char *argv[]) {

  test();

  return 0;
}
