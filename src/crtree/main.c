#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "../file_manager/manager.h"
#include "manager.h"
#include "worker.h"

int main(int argc, char **argv)
{
  InputFile *input = read_file(argv[1]);
  int proceso_inicio = atoi(argv[2]);
  char *r = "R";
  char *m = "M";
  if (strcmp(input->lines[proceso_inicio][0], r) == 0 || strcmp(input->lines[proceso_inicio][0], m) == 0) /* si es manager*/
  {
    manager_init(proceso_inicio, input);
  }
  else
  {
    worker_init(proceso_inicio, input);
  }
  input_file_destroy(input);
}
