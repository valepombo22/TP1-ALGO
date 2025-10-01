<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA LISTA

## Alumno: Valeria Pombo Muzzolón - 112754 - vpombo@fi.uba.ar

---

## Para compilar

gcc -I src -o tp1 main.c src/tp1.c

## Para ejecutar

./tp1 pokedex.csv buscar nombre Pikachu

## Para ejecutar con valgrind

valgrind --leak-check=full ./tp1 pokedex.csv buscar nombre Pikachu

---

## Funcionamiento

El TP implementa un **Tipo de Datos Abstracto (TDA) de lista de Pokémon** que permite:

- Leer Pokémon desde archivos CSV y almacenarlos en un vector dinámico.
- Buscar Pokémon por nombre o ID.
- Recorrer todos los Pokémon mediante un callback.
- Guardar la lista en un archivo CSV.
- Operaciones entre TP: unión, intersección y diferencia.
- Mostrar todos los Pokémon ordenados por nombre o ID usando funciones públicas.

---

## Decisiones de diseño y funcionamiento

### Lectura de archivos

- Cada línea se lee dinámicamente (leer_linea_dinamica).
- Los campos se separan manualmente para evitar strtok.
- Se validan campos, tipo y duplicados antes de agregar al TP.
- Pokémon válidos se agregan al vector dinámico en orden por ID.

### Vector dinámico

- tp1_t contiene un arreglo dinámico (struct pokemon*) con cantidad y capacidad.
- realloc amplía el vector al agregar Pokémon.

### Búsquedas

- Por nombre: lineal.
- Por ID: binaria, gracias al orden por ID.

### Iterador

- tp1_con_cada_pokemon permite ejecutar callbacks sobre todos los Pokémon.

### Operaciones entre TP

- Unión: combina ambos TP evitando duplicados.
- Intersección: conserva solo Pokémon presentes en ambos TP.
- Diferencia: conserva Pokémon solo del primer TP.

### Mostrar ordenado

- Se construye un array de punteros temporal y se ordena con qsort.

### Decisiones importantes

- Duplicación de cadenas (strdup) para cada Pokémon.
- Uso de un auxiliar en realloc para no perder el puntero original.
- Líneas malformadas o duplicadas son ignoradas al leer.
- enum tipo_pokemon para tipos seguros.

---

## Diagramas de memoria y flujo

### 1. Estructura interna del TP1

tp1_t
+-------------------+
| pokemones --------+----> [ struct pokemon ][ struct pokemon ] ...
| cantidad          |       ID, nombre*, tipo, ataque, defensa, velocidad
| capacidad         |
+-------------------+

struct pokemon
+-----------+   +-----------------+
| id        |   | int             |
| nombre ---+---> char*           |
| tipo      |   | enum tipo       |
| ataque    |   | int             |
| defensa   |   | int             |
| velocidad |   | int             |
+-----------+   +-----------------+

---

### 2. Flujo de operaciones

Archivo CSV 
     |
     v
leer_linea_dinamica
     |
     v
parseo campos
     |
     v
validar datos
     |
     v
tp1_agregar_pokemon --> vector interno

---

### 3. Ordenamiento para mostrar

tp1_con_cada_pokemon --> array de punteros
                 |
                 v
               qsort (por nombre o ID)
                 |
                 v
             recorrer para imprimir

---

### 4. Operaciones entre TP

**Unión:**

TP1_A + TP1_B
      |
      v
nuevo TP1 con todos los Pokémon únicos (orden por ID)

**Intersección:**

TP1_A ∩ TP1_B
      |
      v
nuevo TP1 con solo Pokémon presentes en ambos

**Diferencia:**

TP1_A - TP1_B
      |
      v
nuevo TP1 con solo Pokémon de A que no están en B

---

### 5. Array temporal en main

struct pokemon **array = malloc(sizeof(struct pokemon*) * tp1_cantidad(tp));
aux_array aux = { array, 0 };
tp1_con_cada_pokemon(tp, callback_guardar, &aux);
qsort(array, tp1_cantidad(tp), sizeof(struct pokemon*), cmp_nombre); // o cmp_id
for(size_t i = 0; i < tp1_cantidad(tp); i++)
    mostrar_pokemon(array[i]);
free(array);

---

## Respuestas a las preguntas teóricas

### Elección de estructuras

- Vector dinámico: acceso rápido y búsqueda binaria eficiente por ID.
- struct pokemon: campos necesarios + char* para nombre.

### Disposición en memoria

- TP (tp1_t) apunta a un array de Pokémon.
- Cada Pokémon tiene memoria independiente para nombre.
- Arrays auxiliares se usan para ordenar sin tocar la estructura interna.

### Complejidad temporal

Función                       | Complejidad
-------------------------------|------------
tp1_buscar_nombre              | O(n)
tp1_buscar_id                  | O(log n)
tp1_agregar_pokemon            | O(n)
tp1_con_cada_pokemon           | O(n)
tp1_union / interseccion / diferencia | O(n+m)
tp1_guardar_archivo            | O(n)

> n = cantidad de Pokémon en el TP, m = cantidad en otro TP.

### Dificultades en el main

- No acceso a punteros internos → se usa array temporal para ordenar.
- Mostrar por nombre o ID requiere ordenar con qsort.
- Se respetan solo funciones públicas del TP.
