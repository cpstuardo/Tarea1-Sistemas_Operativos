// FUENTE: Curso Aprende C Estructura de datos y algoritmos
#pragma once

struct linked_list_node;
typedef struct linked_list_node Node;

struct linked_list_node
{
  int value;
  Node* next;
};

struct linked_list;
typedef struct linked_list List;
struct linked_list
{
  Node* head;
  Node* tail;
};

// Declaramos las funciones asociadas

/** Inicializa una lista vacía */
List* list_init();
/** Inserta un nuevo elemento al final de la lista */
void list_append(List* list, int value);
/** Imprime todos los elementos de la lista */
void list_print(List* list);
/** Libera todos los recursos asociados a esta lista */
void list_destroy(List* list);