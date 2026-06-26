#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// estrategias de asignacion 
typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} Estrategia;

/* Bloque: nodo de la lista enlazada */
typedef struct Bloque {
    int  inicio;          /* direccion de inicio dentro de la memoria simulada */
    int  tam;             /* tamaño del bloque en bytes                         */
    bool libre;           /* true = libre, false = ocupado                      */
    int  id_proceso;      /* ID del proceso si esta ocupado; -1 si esta libre   */
    struct Bloque *sig;   /* siguiente bloque en la lista                       */
} Bloque;

//memoria 
typedef struct {
    Bloque *cabeza;            /* primer bloque (direccion 0)        */
    int     tam_total;         /* N: tamano total de la memoria      */
    int     procesos_asignados;/* contador de ALLOC exitosos         */
} Memoria;


// crea memoria
Memoria *memoria_init(int N);

//libera lista
void memoria_destruir(Memoria *mem);

// ALLOC
bool asignar(Memoria *mem, int id, int size, Estrategia est);

// free
void liberar(Memoria *mem, int id);

// compact
void compactar(Memoria *mem);

// estado
void imprimir_estado(Memoria *mem);

// reporte final
void imprimir_reporte(Memoria *mem);

#endif
