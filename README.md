<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA LISTA

## Alumno: Valeria Pombo Muzzolón  - 112754 - vpombo@fi.uba.ar

- Para compilar:

```bash
gcc -I src -o main main.c src/tp1.c
```

- Para ejecutar:

```bash
./tp1 archivo_pokemones.txt
```

- Para ejecutar con valgrind:

```bash
valgrind --leak-check=full ./pruebas
```

---

## Funcionamiento

El TP implementa un **TDA de lista dinámica** para manejar un conjunto de pokemones. Cada Pokémon se representa mediante una estructura que contiene su `id` y `nombre`. La lista está implementada mediante un arreglo dinámico que se redimensiona automáticamente cuando se alcanza su capacidad máxima.

### Flujo principal:

1. **Lectura de archivo:**  
   - Se abre el archivo pasado como parámetro.  
   - Se leen las líneas una por una.  
   - Se ignoran líneas vacías o duplicadas.  
   - Se crea un registro `pokemon` para cada línea válida.  
   - Cada Pokémon se agrega al arreglo dinámico, redimensionando si es necesario.

```c
tp1_t *tp1_leer_archivo(const char *nombre_archivo) {
    if (!nombre_archivo)
        return NULL;
    FILE *f = fopen(nombre_archivo, "r");
    if (!f)
        return NULL;

    tp1_t *tp = tp1_crear();
    if (!tp) { fclose(f); return NULL; }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f)) {
        size_t len = strlen(buffer);
        if (len && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        if (buffer[0] == '\0') continue;  // línea vacía
        if (tp1_buscar_nombre(tp, buffer)) continue;  // duplicado

        struct pokemon *p = malloc(sizeof(struct pokemon));
        if (!p) { tp1_destruir(tp); fclose(f); return NULL; }

        p->nombre = mi_strdup(buffer);
        if (!p->nombre) { free(p); tp1_destruir(tp); fclose(f); return NULL; }

        p->id = tp->cantidad + 1;

        if (tp->cantidad == tp->capacidad && !tp1_redimensionar(tp)) {
            free(p->nombre); free(p); tp1_destruir(tp); fclose(f); return NULL;
        }

        tp->pokemones[tp->cantidad++] = p;
    }
    fclose(f);
    return tp;
}
```

2. **Búsqueda:**  
   - `tp1_buscar_nombre` recorre la lista y devuelve el puntero al Pokémon si se encuentra el nombre.  
   - `tp1_buscar_id` devuelve el puntero al Pokémon si se encuentra el id.

3. **Guardar en archivo:**  
   - `tp1_guardar_archivo` recorre la lista y escribe cada Pokémon en el archivo de salida, evitando duplicados.

4. **Operaciones de conjunto:**  
   - `tp1_union`, `tp1_interseccion` y `tp1_diferencia` crean nuevas listas combinando o comparando los pokemones de dos listas, respetando los ids y evitando duplicados.  

5. **Iterador:**  
   - `tp1_con_cada_pokemon` permite recorrer la lista y aplicar una función a cada Pokémon.  
   - La iteración se detiene si la función devuelve `false`.

---

### Diagramas de memoria

#### 1. Lista dinámica de pokemones
<div align="center">
<img width="70%" src="img/diagrama1.svg">
</div>

- Cada bloque del arreglo apunta a una estructura `pokemon`.  
- Se utiliza `realloc` para agrandar el arreglo cuando se llena:

```c
struct pokemon **aux = realloc(tp->pokemones, sizeof(struct pokemon*) * nueva_capacidad);
if (!aux) return false;
tp->pokemones = aux;
tp->capacidad = nueva_capacidad;
```

#### 2. Flujo de lectura de archivo
<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Manejo de errores y punteros NULL

- Todas las funciones verifican punteros NULL antes de operar sobre ellos: lectura de archivos, guardar, buscar y operaciones de conjunto.
- Esto evita crashes y memory leaks.
- Se liberan correctamente todos los punteros en caso de errores parciales.

```c
if (!tp || !nombre_archivo) return NULL;
```

- Pruebas con punteros NULL:
  - `tp1_leer_archivo(NULL)` devuelve NULL. ✅  
  - `tp1_guardar_archivo(NULL, ...)` devuelve NULL. ✅  
  - `tp1_union(NULL, tp1_b)` devuelve NULL. ❌ (corrigido verificando ambos punteros al inicio de la función).  
  - Misma lógica aplicada a `tp1_interseccion` y `tp1_diferencia`.

---

## Resumen de errores detectados y corregidos con valgrind

1. **Memory leaks al crear listas vacías en operaciones de conjunto:**  
   - Se detectaron bloques de 80 y 104 bytes indirectamente perdidos en `tp1_union`, `tp1_interseccion` y `tp1_diferencia`.  
   - Corrección: liberar correctamente las listas temporales cuando los punteros de entrada son NULL o cuando falla la asignación de memoria.

2. **Leaks de cadenas duplicadas (`mi_strdup`):**  
   - Cada Pokémon duplicado generaba memoria reservada que no se liberaba.  
   - Corrección: liberar siempre `p->nombre` si se descarta el Pokémon por duplicado o error de redimensionamiento.

3. **Errores en conteo de Pokémon después de operaciones de unión/diferencia/intersección:**  
   - Se producía que la cantidad de pokemones era mayor o menor que la esperada.  
   - Corrección: verificar duplicados antes de agregar y ajustar correctamente los ids.

4. **Prevención de accesos a NULL:**  
   - Todas las funciones ahora retornan NULL o 0 si se pasan punteros NULL como parámetros.

---

## Decisiones de diseño

- Uso de **arreglo dinámico** en lugar de lista enlazada para un acceso más rápido por índice.  
- Evitar duplicados durante la lectura para mantener consistencia.  
- Modularización de funciones: lectura, escritura, búsqueda, iteración y operaciones de conjunto separadas.  
- Iterador flexible con función callback para cualquier operación sobre los pokemones.



