#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ============================================================
   Estrategias de asignacion (se decide en tiempo de ejecucion)
   ============================================================ */
typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} Estrategia;

/* ============================================================
   Bloque: nodo de la lista enlazada.
   La memoria simulada se representa como una lista de bloques
   ORDENADOS POR DIRECCION (de inicio a fin). Cada bloque puede
   estar libre u ocupado por un proceso.
   ============================================================ */
typedef struct Bloque {
    int  inicio;          /* direccion de inicio dentro de la memoria simulada */
    int  tam;             /* tamano del bloque en bytes                         */
    bool libre;           /* true = libre, false = ocupado                      */
    int  id_proceso;      /* ID del proceso si esta ocupado; -1 si esta libre   */
    struct Bloque *sig;   /* siguiente bloque en la lista                       */
} Bloque;

/* ============================================================
   Memoria: contenedor de la lista + metadatos globales.
   ============================================================ */
typedef struct {
    Bloque *cabeza;            /* primer bloque (direccion 0)        */
    int     tam_total;         /* N: tamano total de la memoria      */
    int     procesos_asignados;/* contador de ALLOC exitosos         */
} Memoria;

/* ---------------- API de la memoria (memoria.c) ---------------- */

/* Crea la memoria con un unico bloque libre [0, N]. */
Memoria *memoria_init(int N);

/* Libera toda la lista al terminar el programa (sin fugas). */
void memoria_destruir(Memoria *mem);

/* ALLOC: intenta asignar 'size' bytes al proceso 'id' segun la
   estrategia. Devuelve true si lo logra, false si no hay espacio. */
bool asignar(Memoria *mem, int id, int size, Estrategia est);

/* FREE: libera TODOS los bloques del proceso 'id' y fusiona
   (coalescencia) los bloques libres vecinos. */
void liberar(Memoria *mem, int id);

/* COMPACT: desplaza lo ocupado hacia la direccion 0. (TODO companera) */
void compactar(Memoria *mem);

/* Imprime el estado tipo: [Libre 100] -> [Ocupado P1 200] -> [Libre 724] */
void imprimir_estado(Memoria *mem);

/* Imprime el reporte final completo (estadisticas + estado). */
void imprimir_reporte(Memoria *mem);

#endif
