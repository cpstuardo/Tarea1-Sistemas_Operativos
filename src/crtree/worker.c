#include "worker.h"
#include "../file_manager/manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <signal.h>

Worker *worker_handler;
pid_t child_pid;
int status;
time_t tiempo_inicio;
time_t tiempo_final;
InputFile *input_file_handler;
int posicion_handler;

void sigabrt_handler(int signal)
{
    /* Termino al hijo y obtengo valores*/
    kill(child_pid, SIGABRT);
    wait(&status);
    worker_handler->return_code = WEXITSTATUS(status);
    time(&tiempo_final);
    worker_handler->t_ejecucion = tiempo_final - tiempo_inicio;
    worker_handler->interruption = 1;
    /* Escribe archivo */
    char nombre_archivo[strlen("aaa.txt")];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "%d.txt", posicion_handler);
    FILE *archivo = fopen(nombre_archivo, "wt");
    fprintf(archivo, "%s, ", input_file_handler->lines[posicion_handler][1]); /* Ejecutable*/
    for (int i = 0; i < worker_handler->n_argumentos; i += 1)                 /* Argumentos*/
    {
        fprintf(archivo, "%s, ", input_file_handler->lines[posicion_handler][2 + i + 1]);
    }
    fprintf(archivo, "%ld, ", worker_handler->t_ejecucion); /* Tiempo ejecución*/
    fprintf(archivo, "%d, ", worker_handler->return_code);  /* Código retorno*/
    fprintf(archivo, "%d", worker_handler->interruption);   /* Interrupción*/
    fclose(archivo);
}

void worker_init(int posicion, InputFile *input)
{
    Worker *worker = malloc(sizeof(Worker));
    worker->id = "W";
    worker->ejecutable = input->lines[posicion][1];
    worker->n_argumentos = atoi(input->lines[posicion][2]);
    worker->t_ejecucion = 0;
    worker->interruption = 0;
    worker->return_code = 0;
    /* Creo la ejecución*/
    pid_t pid = fork();
    signal(SIGINT, SIG_IGN); // ignorar SIGINT
    time(&tiempo_inicio);
    if (pid == 0) /* Child */
    {
        /* Ejecuta el programa */
        printf("Voy a ejecutar el programa: %s\n", input->lines[posicion][1]);
        char *args[worker->n_argumentos + 2];
        args[0] = input->lines[posicion][1];
        for (int i = 1; i <= worker->n_argumentos; i += 1)
        {
            args[i] = input->lines[posicion][2 + i];
        }
        args[worker->n_argumentos + 1] = NULL;
        execv(input->lines[posicion][1], args);
    }
    else /* Parent */
    {
        /* Creo recursos handler */
        signal(SIGABRT, sigabrt_handler);
        input_file_handler = input;
        posicion_handler = posicion;
        worker_handler = worker;
        child_pid = pid;
        /* Espera y guarda valor de retorno y tiempo de ejecución */
        wait(&status);
        worker->return_code = WEXITSTATUS(status);
        time(&tiempo_final);
        worker->t_ejecucion = tiempo_final - tiempo_inicio;
        /* Escribe archivo */
        char nombre_archivo[strlen("aaa.txt")];
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%d.txt", posicion);
        FILE *archivo = fopen(nombre_archivo, "wt");
        fprintf(archivo, "%s, ", input->lines[posicion][1]); /* Ejecutable*/
        for (int i = 0; i < worker->n_argumentos; i += 1)    /* Argumentos*/
        {
            fprintf(archivo, "%s, ", input->lines[posicion][2 + i + 1]);
        }
        fprintf(archivo, "%ld, ", worker->t_ejecucion); /* Tiempo ejecución*/
        fprintf(archivo, "%d, ", worker->return_code);  /* Código retorno*/
        fprintf(archivo, "%d", worker->interruption);   /* Interrupción*/
        fclose(archivo);
    }
    worker_destroy(worker);
}

void worker_destroy(Worker *worker)
{
    free(worker);
};
