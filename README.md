# memsim — Simulador de Politicas de Asignacion de Memoria

Tarea 3 de Sistemas Operativos. Simula el gestor de memoria de un SO usando
particion dinamica, con tres estrategias: First-Fit, Best-Fit y Worst-Fit.
No asigna memoria real: lleva la contabilidad con una lista enlazada de bloques.

## Compilacion

```
make
```

Compila con las flags exigidas por la guia:

```
gcc -Wall -Wextra -std=c17
```

Para limpiar:

```
make clean
```

## Uso

```
./memsim <tam_memoria> <ESTRATEGIA> <archivo_traza>
```

- `tam_memoria`: tamano total de la memoria simulada, en bytes.
- `ESTRATEGIA`: FIRST_FIT | BEST_FIT | WORST_FIT
- `archivo_traza`: ruta a un archivo de texto con las operaciones.

### Formato del archivo de traza

Una operacion por linea:

```
ALLOC <id> <size>   # asigna 'size' bytes al proceso 'id'
FREE <id>           # libera toda la memoria del proceso 'id'
COMPACT             # compacta la memoria
```

## Ejemplo de ejecucion exitoso

Archivo `traza1.txt`:

```
ALLOC 1 100
ALLOC 2 200
ALLOC 3 300
FREE 2
ALLOC 4 150
FREE 1
FREE 4
```

Ejecucion:

```
./memsim 1024 FIRST_FIT traza1.txt
```

Salida:

```
===== Reporte final =====
Procesos asignados: 4
Memoria utilizada: 29.30%
Indice de Fragmentacion Externa: 0.4144
Estado final de la memoria:
[Libre 300] -> [Ocupado P3 300] -> [Libre 424]
```

## Estado de la implementacion

- [x] Lista enlazada de bloques (estructura base)
- [x] Lectura de argumentos y archivo de traza
- [x] ALLOC con First-Fit
- [x] FREE con coalescencia de bloques vecinos
- [x] Reporte final (procesos, % uso, fragmentacion externa, estado)
- [ ] ALLOC con Best-Fit  (TODO)
- [ ] ALLOC con Worst-Fit (TODO)
- [ ] COMPACT             (TODO)
