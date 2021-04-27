#include "manager.h"
#include "linked_list.h"
#include "../file_manager/manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

Manager *manager_handler;
InputFile *input_file_handler;
int posicion_handler;
int status;

void sigabrt_handler_manager(int signal)
{
    /* Manda SIGABRT a sus hijos*/
    Node *current = manager_handler->pid_children->head;
    while (current)
    {
        kill(current->value, SIGABRT);
        current = current->next;
    }
    for (int j = 0; j < manager_handler->n_children; j += 1)
    {
        wait(&status);
    }
    /* Escribe archivo */
    char nombre_archivo[strlen("aaa.txt")];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "%d.txt", posicion_handler);
    FILE *archivo = fopen(nombre_archivo, "wt");
    recorrer_hijos(archivo, input_file_handler, posicion_handler);
    fclose(archivo);
}

void manager_init(int posicion, InputFile *input)
{
    Manager *manager = malloc(sizeof(Manager));
    manager_handler = manager;
    manager->id = input->lines[posicion][0];
    manager->timeout = atoi(input->lines[posicion][1]);
    manager->n_children = atoi(input->lines[posicion][2]);
    manager->children = list_init();
    manager->pid_children = list_init();
    for (int i = 0; i < manager->n_children; i += 1)
    {
        list_append(manager->children, atoi(input->lines[posicion][2 + i + 1]));
    }
    /* Creo a los procesos hijos */
    pid_t pid;
    Node *current = manager->children->head;
    while (current)
    {
        /* Si el hijo es un worker*/
        char *w = "W";
        pid = fork();
        if (strcmp(input->lines[current->value][0], w) == 0)
        {
            if (pid == 0)
            {
                worker_init(current->value, input);
                manager_destroy(manager);
                input_file_destroy(input);
                exit(0);
            }
            else if (pid > 0)
            {
                list_append(manager->pid_children, pid);
                current = current->next;
            }
        }
        /* Si el hijo es manager */
        else
        {
            if (pid == 0)
            {
                manager_init(current->value, input);
                manager_destroy(manager);
                input_file_destroy(input);
                exit(0);
            }
            else if (pid > 0)
            {
                list_append(manager->pid_children, pid);
                current = current->next;
            }
        }
    }
    if (pid > 0)
    {
        /* Manejo de timeout*/
        signal(SIGALRM, sigabrt_handler_manager);
        alarm(manager->timeout);
        /* Manejo SIGINT */
        char *r = "R";
        if (strcmp(manager->id, r) == 0) // Si es raíz
        {
            signal(SIGINT, sigabrt_handler_manager);
        }
        else //Si no es root
        {
            signal(SIGINT, SIG_IGN);
        }
        /* Manejo SIGABRT */
        signal(SIGABRT, sigabrt_handler_manager);
        /* Creo recursos handler */
        input_file_handler = input;
        posicion_handler = posicion;
        /* Padre espera a los hijos*/
        for (int j = 0; j < manager->n_children; j += 1)
        {
            wait(&status);
        }
        /* Escribe archivo */
        char nombre_archivo[strlen("aaa.txt")];
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%d.txt", posicion);
        FILE *archivo = fopen(nombre_archivo, "wt");
        recorrer_hijos(archivo, input, posicion);
        fclose(archivo);
    }
    manager_destroy(manager);
}

void escribir_archivo(FILE *archivo, int posicion)
{
    char nombre_archivo_child[strlen("aaa.txt")];
    snprintf(nombre_archivo_child, sizeof(nombre_archivo_child), "%d.txt", posicion);
    FILE *archivo_lectura = fopen(nombre_archivo_child, "rt");
    if (archivo_lectura == NULL)
    {
        printf("Error en lectura de archivo\n");
    }
    else
    {
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, archivo_lectura))
        {
            buffer[strcspn(buffer, "\r\n")] = 0;
            fprintf(archivo, "%s\n", buffer);
        }
        fclose(archivo_lectura);
    }
}

void recorrer_hijos(FILE *archivo, InputFile *input, int posicion)
{
    char *w = "W";
    if (strcmp(input->lines[posicion][0], w) == 0) // Si es worker
    {
        escribir_archivo(archivo, posicion);
    }
    else // Si es manager
    {
        int cantidad_hijos = atoi(input->lines[posicion][2]);
        for (int i = 3; i < cantidad_hijos + 3; i += 1)
        {
            recorrer_hijos(archivo, input, atoi(input->lines[posicion][i]));
        }
        escribir_archivo(archivo, posicion);
    }
}

void manager_destroy(Manager *manager)
{
    list_destroy(manager->pid_children);
    list_destroy(manager->children);
    free(manager);
};