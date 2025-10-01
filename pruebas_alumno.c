#include "src/tp1.h"
#include "pa2m.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Callback para contar
bool callback_contador(struct pokemon *p, void *extra)
{
	(void)p;
	size_t *contador = (size_t *)extra;
	(*contador)++;
	return true;
}

// Callback para cortar en cierto ID
bool callback_corte(struct pokemon *p, void *extra)
{
	int limite = *((int *)extra);
	// Devuelve true mientras se recorra, false cuando llegue al límite
	return p->id < limite;
}

int main()
{
	pa2m_nuevo_grupo("Lectura de archivos difíciles");

	// Archivo con casos extremos
	FILE *f = fopen("test_pokemones_extremo.csv", "w");
	if (!f)
		return 1;

	fprintf(f, "1,Pikachu,ELEC,55,40,90\n");
	fprintf(f, "2,Charmander,FUEG,52,43,65\n");
	fprintf(f, "3,Bulbasaur,PLAN,49,49,45\n");
	fprintf(f, "4,Squirtle,AGUA,48,65,43\n");
	fprintf(f, "5,Jigglypuff,FANT,45,20,40\n");
	fprintf(f, "malformed line\n"); // línea malformada
	fprintf(f,
		"6,VeryLongPokemonNameThatExceedsNormalLength,NORM,60,50,50\n");
	fprintf(f,
		"3,Bulbasaur,PLAN,99,99,99\n"); // ID duplicado, datos distintos
	fclose(f);

	tp1_t *tp = tp1_leer_archivo("test_pokemones_extremo.csv");
	pa2m_afirmar(tp != NULL, "Archivo leído correctamente");
	pa2m_afirmar(
		tp1_cantidad(tp) == 6, // se ignora línea malformada y duplicado
		"Cantidad de pokemones esperada (ignora errores de línea)");

	pa2m_nuevo_grupo("Lectura de archivo vacío");
	f = fopen("archivo_vacio.csv", "w");
	fclose(f);
	tp1_t *tp_vacio = tp1_leer_archivo("archivo_vacio.csv");
	pa2m_afirmar(tp_vacio != NULL, "Archivo vacío crea TP válido");
	pa2m_afirmar(tp1_cantidad(tp_vacio) == 0,
		     "Archivo vacío genera TP vacío");

	pa2m_nuevo_grupo("Búsquedas difíciles");
	struct pokemon *p = tp1_buscar_nombre(tp, "Pikachu");
	pa2m_afirmar(p && p->id == 1, "Encontrar Pikachu");

	p = tp1_buscar_nombre(tp, "VeryLongPokemonNameThatExceedsNormalLength");
	pa2m_afirmar(p && p->id == 6, "Encontrar Pokémon con nombre largo");

	p = tp1_buscar_nombre(tp, "NoExiste");
	pa2m_afirmar(!p, "No encuentra Pokémon inexistente");

	p = tp1_buscar_id(tp, 3);
	pa2m_afirmar(p && strcmp(p->nombre, "Bulbasaur") == 0,
		     "Encontrar Pokémon por ID");

	p = tp1_buscar_id(tp, 999);
	pa2m_afirmar(!p, "ID inexistente devuelve NULL");

	pa2m_nuevo_grupo("Operaciones TP1 difíciles");
	tp1_t *otro = tp1_leer_archivo("test_pokemones_extremo.csv");

	// Unión
	tp1_t *union_tp = tp1_union(tp, otro);
	pa2m_afirmar(union_tp != NULL, "Union devuelve TP válido");
	pa2m_afirmar(tp1_cantidad(union_tp) >= tp1_cantidad(tp),
		     "Union mantiene o supera la cantidad original");

	// Intersección
	tp1_t *inter_tp = tp1_interseccion(tp, otro);
	pa2m_afirmar(inter_tp != NULL, "Intersección devuelve TP válido");
	pa2m_afirmar(tp1_cantidad(inter_tp) > 0,
		     "Intersección contiene pokemones esperados");

	// Intersección con vacío
	tp1_t *inter_vacio = tp1_interseccion(tp, tp_vacio);
	pa2m_afirmar(inter_vacio && tp1_cantidad(inter_vacio) == 0,
		     "Intersección con TP vacío da vacío");

	// Diferencia
	tp1_t *diff_tp = tp1_diferencia(tp, otro);
	pa2m_afirmar(diff_tp != NULL, "Diferencia devuelve TP válido");

	// Diferencia con TP que contiene todos
	tp1_t *diff_vacio = tp1_diferencia(tp, tp);
	pa2m_afirmar(diff_vacio && tp1_cantidad(diff_vacio) == 0,
		     "Diferencia de TP consigo mismo da vacío");

	pa2m_nuevo_grupo("Guardar y recargar");
	tp1_guardar_archivo(tp, "guardado.csv");
	tp1_t *recargado = tp1_leer_archivo("guardado.csv");
	pa2m_afirmar(recargado && tp1_cantidad(recargado) == tp1_cantidad(tp),
		     "Archivo recargado correctamente");

	// Guardar y recargar vacío
	tp1_guardar_archivo(tp_vacio, "vacio.csv");
	tp1_t *recargado_vacio = tp1_leer_archivo("vacio.csv");
	pa2m_afirmar(recargado_vacio && tp1_cantidad(recargado_vacio) == 0,
		     "Archivo vacío recargado correctamente");

	// Limpiar memoria
	tp1_destruir(tp);
	tp1_destruir(tp_vacio);
	tp1_destruir(otro);
	tp1_destruir(union_tp);
	tp1_destruir(inter_tp);
	tp1_destruir(inter_vacio);
	tp1_destruir(diff_tp);
	tp1_destruir(diff_vacio);
	tp1_destruir(recargado);
	tp1_destruir(recargado_vacio);

	return pa2m_mostrar_reporte();
}
