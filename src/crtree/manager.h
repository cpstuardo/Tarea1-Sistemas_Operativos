#pragma once
#include <stdio.h>
#include "worker.h"
#include "linked_list.h"
#include "../file_manager/manager.h"

struct manager;
typedef struct manager Manager;

struct manager
{
  char* id;
  int timeout;
  int n_children; /* cantidad de hijos*/
  List* children;
  List* pid_children;
};


/* Funciones */
void manager_init(int posicion, InputFile* input_lines);
void escribir_archivo(FILE* archivo, int posicion);
void recorrer_hijos(FILE* archivo, InputFile* input, int posicion);
void manager_destroy(Manager* manager);