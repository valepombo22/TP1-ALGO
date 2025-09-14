#include "pa2m.h"
#include "src/tp1.h"
#include <string.h>

#define ARCHIVO_INEXISTENTE "ejemplos/asdkasjhfskladjhfksdfhksdf.csv"
#define ARCHIVO_VALIDO "ejemplos/normal.csv"
#define ARCHIVO_VALIDO_2 "ejemplos/largo.csv"

void prueba_archivo_inexistente()
{
	tp1_t *tp = tp1_leer_archivo(ARCHIVO_INEXISTENTE);
	pa2m_afirmar(tp == NULL, "Abrir un archivo inexistente devuelve NULL");
}

void prueba_archivo_valido()
{
	tp1_t *tp = tp1_leer_archivo(ARCHIVO_VALIDO);
	pa2m_afirmar(tp != NULL, "Archivo válido se carga correctamente");
	if (tp)
		tp1_destruir(tp);
}

void prueba_buscar_nombre()
{
	tp1_t *tp = tp1_leer_archivo(ARCHIVO_VALIDO);
	struct pokemon *pk = tp1_buscar_nombre(tp, "Pikachu");
	pa2m_afirmar(pk != NULL && pk->id == 1, "Buscar por nombre funciona");
	tp1_destruir(tp);
}

void prueba_buscar_id()
{
	tp1_t *tp = tp1_leer_archivo(ARCHIVO_VALIDO);
	struct pokemon *pk = tp1_buscar_id(tp, 2);
	pa2m_afirmar(pk != NULL && strcmp(pk->nombre, "Charmander") == 0,
		     "Buscar por ID funciona");
	tp1_destruir(tp);
}

void prueba_union()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_VALIDO);
	tp1_t *tp2 = tp1_leer_archivo(ARCHIVO_VALIDO_2);
	tp1_t *res = tp1_union(tp1, tp2);
	pa2m_afirmar(res != NULL && tp1_cantidad(res) >= tp1_cantidad(tp1),
		     "Union de pokedex funciona");
	tp1_destruir(tp1);
	tp1_destruir(tp2);
	tp1_destruir(res);
}

void prueba_interseccion()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_VALIDO);
	tp1_t *tp2 = tp1_leer_archivo(ARCHIVO_VALIDO_2);
	tp1_t *res = tp1_interseccion(tp1, tp2);
	pa2m_afirmar(res != NULL, "Interseccion de pokedex funciona");
	tp1_destruir(tp1);
	tp1_destruir(tp2);
	tp1_destruir(res);
}

void prueba_diferencia()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_VALIDO);
	tp1_t *tp2 = tp1_leer_archivo(ARCHIVO_VALIDO_2);
	tp1_t *res = tp1_diferencia(tp1, tp2);
	pa2m_afirmar(res != NULL, "Diferencia de pokedex funciona");
	tp1_destruir(tp1);
	tp1_destruir(tp2);
	tp1_destruir(res);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas de lectura de archivos");
	prueba_archivo_inexistente();
	prueba_archivo_valido();

	pa2m_nuevo_grupo("Pruebas de búsqueda");
	prueba_buscar_nombre();
	prueba_buscar_id();

	pa2m_nuevo_grupo("Pruebas de operaciones de conjuntos");
	prueba_union();
	prueba_interseccion();
	prueba_diferencia();

	return pa2m_mostrar_reporte();
}
