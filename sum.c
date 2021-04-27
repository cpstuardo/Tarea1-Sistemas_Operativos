#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  sleep(10);
  int sum = atoi(argv[1]) + atoi(argv[2]);
  return sum;
}