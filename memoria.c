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
    (void)mem; (void)size;
    /* TODO (companera): recorrer toda la lista y quedarse con el
       bloque libre cuyo tam sea >= size y el MENOR posible. */
    fprintf(stderr, "[TODO] Best-Fit aun no implementado\n");
    return NULL;
}

/* Worst-Fit: el bloque libre MAS GRANDE disponible. */
static Bloque *buscar_worst_fit(Memoria *mem, int size) {
    (void)mem; (void)size;
    /* TODO (companera): recorrer toda la lista y quedarse con el
       bloque libre de MAYOR tam (siempre que sea >= size). */
    fprintf(stderr, "[TODO] Worst-Fit aun no implementado\n");
    return NULL;
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
    (void)mem;
    /* TODO (companera): desplazar todos los bloques ocupados hacia
       la direccion 0 (sin huecos entre ellos) y dejar TODO el espacio
       libre consolidado en un unico bloque al final. */
    fprintf(stderr, "[TODO] COMPACT aun no implementado\n");
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
