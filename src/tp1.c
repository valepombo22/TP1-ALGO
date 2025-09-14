#include "tp1.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tp1 {
	struct pokemon *pokemones;
	size_t cantidad;
	size_t capacidad;
};

static enum tipo_pokemon tipo_desde_string(const char *str)
{
	if (strcmp(str, "ELEC") == 0)
		return TIPO_ELEC;
	if (strcmp(str, "FUEG") == 0)
		return TIPO_FUEG;
	if (strcmp(str, "PLAN") == 0)
		return TIPO_PLAN;
	if (strcmp(str, "AGUA") == 0)
		return TIPO_AGUA;
	if (strcmp(str, "NORM") == 0)
		return TIPO_NORM;
	if (strcmp(str, "FANT") == 0)
		return TIPO_FANT;
	if (strcmp(str, "PSI") == 0)
		return TIPO_PSI;
	if (strcmp(str, "LUCH") == 0)
		return TIPO_LUCH;
	return TIPO_NORM;
}

static bool existe_id(tp1_t *tp, int id)
{
	size_t i;
	for (i = 0; i < tp->cantidad; i++) {
		if (tp->pokemones[i].id == id)
			return true;
	}
	return false;
}

size_t tp1_cantidad(tp1_t *tp)
{
	if (!tp)
		return 0;
	return tp->cantidad;
}

tp1_t *tp1_leer_archivo(const char *nombre_archivo)
{
	FILE *f = fopen(nombre_archivo, "r");
	if (!f)
		return NULL;

	tp1_t *tp = malloc(sizeof(tp1_t));
	if (!tp) {
		fclose(f);
		return NULL;
	}

	tp->pokemones = NULL;
	tp->cantidad = 0;
	tp->capacidad = 0;

	char linea[256];

	while (fgets(linea, sizeof(linea), f)) {
		int id, ataque, defensa, velocidad;
		char nombre[256];
		char tipo_str[16];

		int leidos = sscanf(linea, "%d,%255[^,],%15[^,],%d,%d,%d", &id,
				    nombre, tipo_str, &ataque, &defensa,
				    &velocidad);

		bool valido = (leidos == 6) && !existe_id(tp, id);

		if (valido) {
			if (tp->cantidad == tp->capacidad) {
				size_t nueva_capacidad =
					(tp->capacidad == 0) ?
						4 :
						tp->capacidad * 2;
				struct pokemon *nuevo_array =
					realloc(tp->pokemones,
						nueva_capacidad *
							sizeof(struct pokemon));
				if (!nuevo_array) {
					tp1_destruir(tp);
					fclose(f);
					return NULL;
				}
				tp->pokemones = nuevo_array;
				tp->capacidad = nueva_capacidad;
			}

			nombre[strcspn(nombre, "\r\n")] = '\0';
			tipo_str[strcspn(tipo_str, "\r\n")] = '\0';

			struct pokemon *pk = &tp->pokemones[tp->cantidad];
			pk->id = id;
			pk->nombre = strdup(nombre);
			pk->tipo = tipo_desde_string(tipo_str);
			pk->ataque = ataque;
			pk->defensa = defensa;
			pk->velocidad = velocidad;

			tp->cantidad++;
		}
	}

	fclose(f);
	return tp;
}

tp1_t *tp1_guardar_archivo(tp1_t *tp, const char *nombre_archivo)
{
	if (!tp || !nombre_archivo)
		return NULL;

	FILE *f = fopen(nombre_archivo, "w");
	if (!f)
		return NULL;

	for (size_t i = 0; i < tp->cantidad; i++) {
		struct pokemon *p = &tp->pokemones[i];

		const char *tipo_str;
		if (p->tipo == TIPO_ELEC)
			tipo_str = "ELEC";
		else if (p->tipo == TIPO_FUEG)
			tipo_str = "FUEG";
		else if (p->tipo == TIPO_PLAN)
			tipo_str = "PLAN";
		else if (p->tipo == TIPO_AGUA)
			tipo_str = "AGUA";
		else if (p->tipo == TIPO_NORM)
			tipo_str = "NORM";
		else if (p->tipo == TIPO_FANT)
			tipo_str = "FANT";
		else if (p->tipo == TIPO_PSI)
			tipo_str = "PSI";
		else if (p->tipo == TIPO_LUCH)
			tipo_str = "LUCH";
		else
			tipo_str = "NORM";

		fprintf(f, "%d,%s,%s,%d,%d,%d\n", p->id, p->nombre, tipo_str,
			p->ataque, p->defensa, p->velocidad);
	}

	fclose(f);
	return tp;
}

struct pokemon *tp1_buscar_nombre(tp1_t *tp, const char *nombre)
{
	if (!tp || !nombre)
		return NULL;

	size_t i;
	for (i = 0; i < tp->cantidad; i++) {
		if (strcmp(tp->pokemones[i].nombre, nombre) == 0)
			return &tp->pokemones[i];
	}

	return NULL;
}

struct pokemon *tp1_buscar_id(tp1_t *tp, int id)
{
	if (!tp)
		return NULL;

	size_t i;

	for (i = 0; i < tp->cantidad; i++) {
		if (tp->pokemones[i].id == id)
			return &tp->pokemones[i];
	}

	return NULL;
}

tp1_t *tp1_union(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = malloc(sizeof(tp1_t));
	if (!nuevo)
		return NULL;

	nuevo->pokemones = NULL;
	nuevo->cantidad = 0;
	nuevo->capacidad = 0;

	size_t i;
	for (i = 0; i < un_tp->cantidad; i++) {
		struct pokemon *pk = &un_tp->pokemones[i];

		if (nuevo->cantidad == nuevo->capacidad) {
			size_t nueva_cap = (nuevo->capacidad == 0) ?
						   4 :
						   nuevo->capacidad * 2;
			struct pokemon *aux =
				realloc(nuevo->pokemones,
					nueva_cap * sizeof(struct pokemon));
			if (!aux) {
				tp1_destruir(nuevo);
				return NULL;
			}
			nuevo->pokemones = aux;
			nuevo->capacidad = nueva_cap;
		}

		struct pokemon *p = &nuevo->pokemones[nuevo->cantidad];
		p->id = pk->id;
		p->nombre = strdup(pk->nombre);
		p->tipo = pk->tipo;
		p->ataque = pk->ataque;
		p->defensa = pk->defensa;
		p->velocidad = pk->velocidad;

		nuevo->cantidad++;
	}

	for (i = 0; i < otro_tp->cantidad; i++) {
		struct pokemon *pk = &otro_tp->pokemones[i];

		bool existe = false;
		size_t j;
		for (j = 0; j < nuevo->cantidad; j++) {
			if (nuevo->pokemones[j].id == pk->id) {
				existe = true;
			}
		}

		if (!existe) {
			if (nuevo->cantidad == nuevo->capacidad) {
				size_t nueva_cap = (nuevo->capacidad == 0) ?
							   4 :
							   nuevo->capacidad * 2;
				struct pokemon *aux = realloc(
					nuevo->pokemones,
					nueva_cap * sizeof(struct pokemon));
				if (!aux) {
					tp1_destruir(nuevo);
					return NULL;
				}
				nuevo->pokemones = aux;
				nuevo->capacidad = nueva_cap;
			}

			struct pokemon *p = &nuevo->pokemones[nuevo->cantidad];
			p->id = pk->id;
			p->nombre = strdup(pk->nombre);
			p->tipo = pk->tipo;
			p->ataque = pk->ataque;
			p->defensa = pk->defensa;
			p->velocidad = pk->velocidad;

			nuevo->cantidad++;
		}
	}

	return nuevo;
}

tp1_t *tp1_interseccion(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = malloc(sizeof(tp1_t));
	if (!nuevo)
		return NULL;

	nuevo->pokemones = NULL;
	nuevo->cantidad = 0;
	nuevo->capacidad = 0;

	size_t i;
	for (i = 0; i < un_tp->cantidad; i++) {
		struct pokemon *pk = &un_tp->pokemones[i];

		bool existe = false;
		size_t j;
		for (j = 0; j < otro_tp->cantidad; j++) {
			if (pk->id == otro_tp->pokemones[j].id) {
				existe = true;
			}
		}

		if (existe) {
			if (nuevo->cantidad == nuevo->capacidad) {
				size_t nueva_cap = (nuevo->capacidad == 0) ?
							   4 :
							   nuevo->capacidad * 2;
				struct pokemon *aux = realloc(
					nuevo->pokemones,
					nueva_cap * sizeof(struct pokemon));
				if (!aux) {
					tp1_destruir(nuevo);
					return NULL;
				}
				nuevo->pokemones = aux;
				nuevo->capacidad = nueva_cap;
			}

			struct pokemon *p = &nuevo->pokemones[nuevo->cantidad];
			p->id = pk->id;
			p->nombre = strdup(pk->nombre);
			p->tipo = pk->tipo;
			p->ataque = pk->ataque;
			p->defensa = pk->defensa;
			p->velocidad = pk->velocidad;

			nuevo->cantidad++;
		}
	}

	return nuevo;
}

tp1_t *tp1_diferencia(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = malloc(sizeof(tp1_t));
	if (!nuevo)
		return NULL;

	nuevo->pokemones = NULL;
	nuevo->cantidad = 0;
	nuevo->capacidad = 0;

	size_t i;
	for (i = 0; i < un_tp->cantidad; i++) {
		struct pokemon *pk = &un_tp->pokemones[i];

		bool existe = false;
		size_t j;
		for (j = 0; j < otro_tp->cantidad; j++) {
			if (pk->id == otro_tp->pokemones[j].id) {
				existe = true;
			}
		}

		if (!existe) {
			if (nuevo->cantidad == nuevo->capacidad) {
				size_t nueva_cap = (nuevo->capacidad == 0) ?
							   4 :
							   nuevo->capacidad * 2;
				struct pokemon *aux = realloc(
					nuevo->pokemones,
					nueva_cap * sizeof(struct pokemon));
				if (!aux) {
					tp1_destruir(nuevo);
					return NULL;
				}
				nuevo->pokemones = aux;
				nuevo->capacidad = nueva_cap;
			}

			struct pokemon *p = &nuevo->pokemones[nuevo->cantidad];
			p->id = pk->id;
			p->nombre = strdup(pk->nombre);
			p->tipo = pk->tipo;
			p->ataque = pk->ataque;
			p->defensa = pk->defensa;
			p->velocidad = pk->velocidad;

			nuevo->cantidad++;
		}
	}
	return nuevo;
}

size_t tp1_con_cada_pokemon(tp1_t *tp, bool (*f)(struct pokemon *, void *),
			    void *extra)
{
	if (!tp || !f)
		return 0;

	size_t cont = 0;
	size_t i;
	for (i = 0; i < tp->cantidad; i++) {
		if (!f(&tp->pokemones[i], extra)) {
			return cont;
		}
		cont++;
	}
	return cont;
}

void tp1_destruir(tp1_t *tp)
{
	if (!tp)
		return;
	size_t i;
	for (i = 0; i < tp->cantidad; i++) {
		free(tp->pokemones[i].nombre);
	}
	free(tp->pokemones);
	free(tp);
}
