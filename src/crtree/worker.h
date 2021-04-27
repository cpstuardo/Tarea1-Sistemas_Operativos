#pragma once
#include <stdio.h>
#include <time.h>
#include "../file_manager/manager.h"

struct worker;
typedef struct worker Worker;

struct worker
{
  char* id;
  char* ejecutable;
  int n_argumentos;
  time_t t_ejecucion;
  int interruption;
  int return_code;
};

/* Funciones */
void worker_init(int posicion, InputFile* input);
void worker_destroy(Worker* worker);
