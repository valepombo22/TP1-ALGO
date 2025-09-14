#include "tp1.h"
#include <stdio.h>
#include <string.h>

bool mostrar_pokemon(struct pokemon *pk, void *extra) {
  printf("%d - %s (%d,%d,%d)\n", pk->id, pk->nombre, pk->ataque, pk->defensa,
         pk->velocidad);
  return true;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf(
        "Uso: %s <archivo1.csv> <operacion> [archivo2.csv] [resultado.csv]\n",
        argv[0]);
    return 1;
  }

  const char *archivo1 = argv[1];
  const char *operacion = argv[2];

  tp1_t *tp1 = tp1_leer_archivo(archivo1);
  if (!tp1) {
    printf("Error al leer archivo %s\n", archivo1);
    return 1;
  }

  if (strcmp(operacion, "mostrar") == 0) {

    tp1_con_cada_pokemon(tp1, mostrar_pokemon, NULL);
  } else if (argc >= 5) {
    const char *archivo2 = argv[3];
    const char *resultado = argv[4];

    tp1_t *tp2 = tp1_leer_archivo(archivo2);
    if (!tp2) {
      printf("Error al leer archivo %s\n", archivo2);
      tp1_destruir(tp1);
      return 1;
    }

    tp1_t *res = NULL;

    if (strcmp(operacion, "union") == 0)
      res = tp1_union(tp1, tp2);
    else if (strcmp(operacion, "interseccion") == 0)
      res = tp1_interseccion(tp1, tp2);
    else if (strcmp(operacion, "diferencia") == 0)
      res = tp1_diferencia(tp1, tp2);
    else {
      printf("Operacion desconocida\n");
      tp1_destruir(tp1);
      tp1_destruir(tp2);
      return 1;
    }

    if (res) {
      tp1_guardar_archivo(res, resultado);
      printf("Operacion %s realizada, guardada en %s\n", operacion, resultado);
      tp1_destruir(res);
    }

    tp1_destruir(tp2);
  } else {
    printf("Parametros insuficientes para operacion %s\n", operacion);
  }

  tp1_destruir(tp1);
  return 0;
}
