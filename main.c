#include "tipos.h"

/* memsim: simulador de politicas de asignacion de memoria.
   Uso:  ./memsim <tam_memoria> <ESTRATEGIA> <archivo_traza>
   Ej:   ./memsim 1024 FIRST_FIT traza1.txt                    */

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <tam_memoria> <ESTRATEGIA> <archivo_traza>\n", argv[0]);
        fprintf(stderr, "  ESTRATEGIA: FIRST_FIT | BEST_FIT | WORST_FIT\n");
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Tamano de memoria invalido: %s\n", argv[1]);
        return 1;
    }

    Estrategia est;
    if      (strcmp(argv[2], "FIRST_FIT") == 0) est = FIRST_FIT;
    else if (strcmp(argv[2], "BEST_FIT")  == 0) est = BEST_FIT;
    else if (strcmp(argv[2], "WORST_FIT") == 0) est = WORST_FIT;
    else {
        fprintf(stderr, "Estrategia desconocida: %s\n", argv[2]);
        fprintf(stderr, "Use FIRST_FIT | BEST_FIT | WORST_FIT\n");
        return 1;
    }

    FILE *f = fopen(argv[3], "r");
    if (f == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo de traza: %s\n", argv[3]);
        return 1;
    }

    Memoria *mem = memoria_init(N);

    char linea[256];
    while (fgets(linea, sizeof(linea), f) != NULL) {
        char op[32];
        if (sscanf(linea, "%31s", op) != 1)
            continue;  /* linea vacia o solo espacios */

        if (strcmp(op, "ALLOC") == 0) {
            int id, size;
            if (sscanf(linea, "%*s %d %d", &id, &size) != 2) {
                fprintf(stderr, "Linea ALLOC mal formada: %s", linea);
                continue;
            }
            if (!asignar(mem, id, size, est)) {
                /* Fin anticipado: ALLOC fallo por fragmentacion. */
                printf("ALLOC fallido: P%d pidio %d bytes y no hay bloque suficiente.\n", id, size);
                printf("La simulacion termina de forma anticipada.\n");
                break;
            }

        } else if (strcmp(op, "FREE") == 0) {
            int id;
            if (sscanf(linea, "%*s %d", &id) != 1) {
                fprintf(stderr, "Linea FREE mal formada: %s", linea);
                continue;
            }
            liberar(mem, id);

        } else if (strcmp(op, "COMPACT") == 0) {
            compactar(mem);

        } else {
            fprintf(stderr, "Operacion desconocida: %s", linea);
        }
    }

    fclose(f);

    imprimir_reporte(mem);
    memoria_destruir(mem);
    return 0;
}
