# Tarea3-SO: memsim
Un simulador por consola que modela el comportamiento del
gestor de memoria de un sistema operativo utilizando distintas estrategias de partición dinámica.

##

### Compilación

```gcc -Wall -Wextra -std=c17 -o memsim main.c memoria.c```

### Uso

```./memsim <tam_memoria> <ESTRATEGIA> <archivo_traza>```

- ```tam_memoria```: tamaño total de la memoria simulada en bytes (ej: 1024)
- ```ESTRATEGIA```: FIRST_FIT, BEST_FIT o WORST_FIT
- ```archivo_traza```: archivo .txt con operaciones ALLOC, FREE, COMPACT

### Salida esperada (traza1.txt con FIRST_FIT, memoria=1024):
```
===== Reporte final =====
Procesos asignados: 4
Memoria utilizada: 0.00%
Indice de Fragmentacion Externa: 0.0000
Estado final de la memoria:
[Libre 1024]
```

### Archivos

Archivo | Descripción
--- | --- 
tipos.h | Definición de structs y prototipos
memoria.c | Implementación de la lista enlazada y estrategias
main.c | Parseo de argumentos y loop del trace file
traza1.txt | Traza básica de prueba
traza2.txt | Traza con COMPACT y casos borde
