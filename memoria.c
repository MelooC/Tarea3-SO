#include "tipos.h"

/* ============================================================
   memoria_init: arranca con TODA la memoria como un solo bloque libre.
   ============================================================ */
Memoria *memoria_init(int N) {
    Memoria *mem = malloc(sizeof(Memoria));
    Bloque  *b   = malloc(sizeof(Bloque));

    b->inicio     = 0;
    b->tam        = N;
    b->libre      = true;
    b->id_proceso = -1;
    b->sig        = NULL;

    mem->cabeza             = b;
    mem->tam_total          = N;
    mem->procesos_asignados = 0;
    return mem;
}

void memoria_destruir(Memoria *mem) {
    if (mem == NULL) return;
    Bloque *b = mem->cabeza;
    while (b != NULL) {
        Bloque *sig = b->sig;
        free(b);
        b = sig;
    }
    free(mem);
}

/* ============================================================
   Busqueda del bloque a usar, segun la estrategia.
   Por ahora solo First-Fit esta implementado; Best/Worst son
   ganchos (stubs) claramente marcados para que los completes.
   ============================================================ */

/* First-Fit: el PRIMER bloque libre (desde el inicio) con tam >= size. */
static Bloque *buscar_first_fit(Memoria *mem, int size) {
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (b->libre && b->tam >= size)
            return b;
    }
    return NULL;
}

/* Best-Fit: el bloque libre MAS PEQUENO que igual sirva. */
static Bloque *buscar_best_fit(Memoria *mem, int size) {
        Bloque *mejor = NULL;
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (b->libre && b->tam >= size) {
            //se guarda este su aun no hay candidato o si 
            //es más pequeño que el mejor actual
            if (mejor == NULL || b->tam < mejor->tam)
                mejor = b;
        }
    }
    return mejor;  /* NULL si no hubo ninguno adecuado */
}


/* Worst-Fit: el bloque libre MAS GRANDE disponible. */
static Bloque *buscar_worst_fit(Memoria *mem, int size) {
        Bloque *peor = NULL;
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (b->libre && b->tam >= size) {
            // se guarda este si aun no hay candidato, o
            //si es más grande que el peor actual
            if (peor == NULL || b->tam > peor->tam)
                peor = b;
        }
    }
    return peor;  /* NULL si no hubo ninguno adecuado */
}


static Bloque *buscar_bloque(Memoria *mem, int size, Estrategia est) {
    switch (est) {
        case FIRST_FIT: return buscar_first_fit(mem, size);
        case BEST_FIT:  return buscar_best_fit(mem, size);
        case WORST_FIT: return buscar_worst_fit(mem, size);
    }
    return NULL;
}

/* ============================================================
   asignar (ALLOC): busca un hueco y, si lo encuentra, lo parte
   en una parte ocupada + el sobrante libre.
   ============================================================ */
bool asignar(Memoria *mem, int id, int size, Estrategia est) {
    if (size <= 0) return false;

    Bloque *b = buscar_bloque(mem, size, est);
    if (b == NULL) return false;   /* no hay espacio -> ALLOC falla */

    /* Si sobra espacio, creamos un bloque libre con el resto,
       justo despues del bloque que vamos a ocupar. */
    if (b->tam > size) {
        Bloque *resto = malloc(sizeof(Bloque));
        resto->inicio     = b->inicio + size;
        resto->tam        = b->tam - size;
        resto->libre      = true;
        resto->id_proceso = -1;
        resto->sig        = b->sig;
        b->sig            = resto;
    }

    b->tam        = size;
    b->libre      = false;
    b->id_proceso = id;
    mem->procesos_asignados++;
    return true;
}

/* ============================================================
   coalescer: fusiona bloques LIBRES vecinos en uno solo.
   ============================================================ */
static void coalescer(Memoria *mem) {
    Bloque *b = mem->cabeza;
    while (b != NULL && b->sig != NULL) {
        if (b->libre && b->sig->libre) {
            Bloque *muerto = b->sig;
            b->tam += muerto->tam;      /* el primero absorbe al segundo */
            b->sig  = muerto->sig;
            free(muerto);
            /* no avanzamos: seguimos fusionando con el nuevo vecino */
        } else {
            b = b->sig;
        }
    }
}

/* ============================================================
   liberar (FREE): marca como libres TODOS los bloques del
   proceso 'id' y luego coalesce.
   ============================================================ */
void liberar(Memoria *mem, int id) {
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (!b->libre && b->id_proceso == id) {
            b->libre      = true;
            b->id_proceso = -1;
        }
    }
    coalescer(mem);
}

/* ============================================================
   compactar (COMPACT): TODO companera.
   ============================================================ */
void compactar(Memoria *mem) {
    
    /* 1. cuanta memoria esta ocupada en total. */
    int total_ocupado = 0;
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (!b->libre)
            total_ocupado += b->tam;
    }

        /* 2. reconstruir la lista desde cero. 
        recorre la lista y se queda solo con los bloques ocupados, reasignando sus direcciones de inicio de forma contigua desde 0.
        luego se liberan los nodos que eran libres */

    Bloque *nueva_cabeza = NULL;
    Bloque *nueva_cola   = NULL;
    int cursor = 0;   /* direccion de inicio del próximo bloque ocupado */
 
    Bloque *b = mem->cabeza;
    while (b != NULL) {
        Bloque *sig = b->sig;
        if (!b->libre) {
            /* se reasigna la dirección de inicio y se encadena */
            b->inicio = cursor;
            cursor   += b->tam;
            b->sig    = NULL;
 
            if (nueva_cola == NULL) {
                nueva_cabeza = b;
                nueva_cola   = b;
            } else {
                nueva_cola->sig = b;
                nueva_cola      = b;
            }
        } else {
            /* yy descartamos bloque libre (ya estaba reemplazado con
               un unico bloque libre al final) */
            free(b);
        }
        b = sig;
    }


    /* 3. si queda espacio libre, agregar un unico bloque libre al final */
    int espacio_libre = mem->tam_total - total_ocupado;
    if (espacio_libre > 0) {
        Bloque *libre = malloc(sizeof(Bloque));
        libre->inicio     = cursor;
        libre->tam        = espacio_libre;
        libre->libre      = true;
        libre->id_proceso = -1;
        libre->sig        = NULL;
 
        if (nueva_cola == NULL) {
            /* si no había bloques ocupados, toda la memoria queda libre */
            nueva_cabeza = libre;
        } else {
            nueva_cola->sig = libre;
        }
    }
 
    /* 4. si no habia nada ocupado Y no habia espacio libre*/
    mem->cabeza = nueva_cabeza;
}



/* ============================================================
   Salida
   ============================================================ */
void imprimir_estado(Memoria *mem) {
    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (b->libre)
            printf("[Libre %d]", b->tam);
        else
            printf("[Ocupado P%d %d]", b->id_proceso, b->tam);
        if (b->sig != NULL) printf(" -> ");
    }
    printf("\n");
}

void imprimir_reporte(Memoria *mem) {
    int usados = 0, libres = 0, bmax = 0;

    for (Bloque *b = mem->cabeza; b != NULL; b = b->sig) {
        if (b->libre) {
            libres += b->tam;
            if (b->tam > bmax) bmax = b->tam;
        } else {
            usados += b->tam;
        }
    }

    double pct_uso = (mem->tam_total > 0)
                   ? (100.0 * usados / mem->tam_total) : 0.0;

    /* Fext = 1 - Bmax / Mlibre. Si no hay memoria libre, no hay
       fragmentacion externa que medir -> 0. */
    double fext = (libres > 0)
                ? (1.0 - (double)bmax / libres) : 0.0;

    printf("\n===== Reporte final =====\n");
    printf("Procesos asignados: %d\n", mem->procesos_asignados);
    printf("Memoria utilizada: %.2f%%\n", pct_uso);
    printf("Indice de Fragmentacion Externa: %.4f\n", fext);
    printf("Estado final de la memoria:\n");
    imprimir_estado(mem);
}
