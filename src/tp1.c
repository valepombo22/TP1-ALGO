#include "tp1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define TIPO_INVALIDO ((enum tipo_pokemon) - 1)

struct tp1 {
	struct pokemon *pokemones;
	size_t cantidad;
	size_t capacidad;
};

//(strdup)
static char *duplicar_cadena(const char *s)
{
	if (!s)
		return NULL;
	size_t len = strlen(s);
	char *copia = malloc(len + 1);
	if (!copia)
		return NULL;
	strcpy(copia, s);
	return copia;
}

static struct pokemon copiar_pokemon(const struct pokemon *p)
{
	struct pokemon nuevo = *p;
	nuevo.nombre = duplicar_cadena(p->nombre);
	return nuevo;
}

char *leer_linea_dinamica(FILE *f)
{
	size_t capacidad = 128;
	size_t len = 0;
	char *buffer = malloc(capacidad);
	if (!buffer)
		return NULL;

	int c;
	while ((c = fgetc(f)) != EOF) {
		if (len + 1 >= capacidad) {
			capacidad *= 2;
			char *tmp = realloc(buffer, capacidad);
			if (!tmp) {
				free(buffer);
				return NULL;
			}
			buffer = tmp;
		}
		if (c == '\n')
			break;
		buffer[len++] = (char)c;
	}

	if (len == 0 && c == EOF) {
		free(buffer);
		return NULL;
	}

	buffer[len] = '\0';
	return buffer;
}

static enum tipo_pokemon tipo_from_string(const char *str)
{
	if (!str || str[0] == '\0')
		return TIPO_INVALIDO;

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

	return TIPO_INVALIDO;
}

static tp1_t *tp1_crear()
{
	tp1_t *tp1 = malloc(sizeof(tp1_t));
	if (!tp1)
		return NULL;
	tp1->cantidad = 0;
	tp1->capacidad = 8;
	tp1->pokemones = malloc(sizeof(struct pokemon) * tp1->capacidad);
	if (!tp1->pokemones) {
		free(tp1);
		return NULL;
	}
	return tp1;
}

bool tp1_agregar_pokemon(tp1_t *tp, struct pokemon p)
{
	if (!tp)
		return false;

	if (tp1_buscar_id(tp, p.id) != NULL)
		return false;

	struct pokemon *nuevo_array = realloc(
		tp->pokemones, (tp->cantidad + 1) * sizeof(struct pokemon));
	if (!nuevo_array)
		return false;

	tp->pokemones = nuevo_array;

	size_t i = tp->cantidad;
	while (i > 0 && tp->pokemones[i - 1].id > p.id) {
		tp->pokemones[i] = tp->pokemones[i - 1];
		i--;
	}

	tp->pokemones[i] = p;
	tp->cantidad++;

	return true;
}

//CREACIÓN Y DESTRUCCIÓN
//Crear y llenar el TP1 desde un archivo
tp1_t *tp1_leer_archivo(const char *nombre_archivo)
{
	if (!nombre_archivo)
		return NULL;

	FILE *f = fopen(nombre_archivo, "r");
	if (!f)
		return NULL;

	tp1_t *tp1 = tp1_crear();
	if (!tp1) {
		fclose(f);
		return NULL;
	}

	char *linea = NULL;

	while ((linea = leer_linea_dinamica(f)) != NULL) {
		linea[strcspn(linea, "\r\n")] = 0;

		char *campos[6] = { NULL };
		size_t campo = 0;
		char *inicio = linea;

		for (char *p = linea; *p; p++) {
			if (*p == ',') {
				*p = '\0';
				if (campo < 6)
					campos[campo++] = inicio;
				inicio = p + 1;
			}
		}

		if (campo < 6)
			campos[campo++] = inicio;

		if (campo != 6) {
			free(linea);
			continue;
		}

		bool invalida = false;
		for (size_t i = 0; i < 6; i++)
			if (!campos[i] || campos[i][0] == '\0')
				invalida = true;
		if (invalida) {
			free(linea);
			continue;
		}

		struct pokemon p;
		p.id = atoi(campos[0]);
		p.nombre = duplicar_cadena(campos[1]);
		p.tipo = tipo_from_string(campos[2]);
		p.ataque = atoi(campos[3]);
		p.defensa = atoi(campos[4]);
		p.velocidad = atoi(campos[5]);

		if (p.tipo == TIPO_INVALIDO || !p.nombre) {
			free(p.nombre);
			free(linea);
			continue;
		}

		if (tp1_buscar_id(tp1, p.id) != NULL) {
			free(p.nombre);
			free(linea);
			continue;
		}

		if (!tp1_agregar_pokemon(tp1, p)) {
			free(p.nombre);
			free(linea);
			break;
		}

		free(linea);
	}

	fclose(f);
	return tp1;
}

//Liberar la memoria
void tp1_destruir(tp1_t *tp)
{
	if (!tp)
		return;

	for (size_t i = 0; i < tp->cantidad; i++) {
		free(tp->pokemones[i].nombre);
	}

	free(tp->pokemones);
	free(tp);
}

//INFORMACIÓN
//Cantidad de pokemones
size_t tp1_cantidad(tp1_t *tp1)
{
	if (!tp1)
		return 0;
	return tp1->cantidad;
}

//Buscar por nombre
struct pokemon *tp1_buscar_nombre(tp1_t *tp, const char *nombre)
{
	if (!tp || !nombre)
		return NULL;

	size_t i;
	for (i = 0; i < tp->cantidad; i++) {
		if (strcmp(tp->pokemones[i].nombre, nombre) == 0) {
			return &tp->pokemones[i];
		}
	}

	return NULL;
}

//Buscar por id
struct pokemon *tp1_buscar_id(tp1_t *tp, int id)
{
	if (!tp || tp->cantidad == 0)
		return NULL;

	size_t izq = 0;
	size_t der = tp->cantidad - 1;

	while (izq <= der) {
		size_t medio = izq + (der - izq) / 2;
		int actual = tp->pokemones[medio].id;

		if (actual == id) {
			return &tp->pokemones[medio];
		} else if (actual < id) {
			izq = medio + 1;
		} else {
			if (medio == 0)
				break;
			der = medio - 1;
		}
	}

	return NULL;
}

//RECORRIDO
size_t tp1_con_cada_pokemon(tp1_t *un_tp, bool (*f)(struct pokemon *, void *),
			    void *extra)
{
	if (!un_tp || !f)
		return 0;

	size_t cont = 0;
	for (size_t i = 0; i < (size_t)un_tp->cantidad; i++) {
		cont++;
		if (!f(&un_tp->pokemones[i], extra)) {
			break;
		}
	}
	return cont;
}

//GUARDADO
tp1_t *tp1_guardar_archivo(tp1_t *tp1, const char *nombre)
{
	if (!tp1 || !nombre)
		return NULL;

	FILE *f = fopen(nombre, "w");
	if (!f)
		return NULL;

	for (size_t i = 0; i < tp1->cantidad; i++) {
		struct pokemon *p = &tp1->pokemones[i];
		const char *tipo_str;

		switch (p->tipo) {
		case TIPO_ELEC:
			tipo_str = "ELEC";
			break;
		case TIPO_FUEG:
			tipo_str = "FUEG";
			break;
		case TIPO_PLAN:
			tipo_str = "PLAN";
			break;
		case TIPO_AGUA:
			tipo_str = "AGUA";
			break;
		case TIPO_NORM:
			tipo_str = "NORM";
			break;
		case TIPO_FANT:
			tipo_str = "FANT";
			break;
		case TIPO_PSI:
			tipo_str = "PSI";
			break;
		case TIPO_LUCH:
			tipo_str = "LUCH";
			break;
		default:
			tipo_str = "NORM";
			break;
		}

		if (fprintf(f, "%d,%s,%s,%d,%d,%d\n", p->id, p->nombre,
			    tipo_str, p->ataque, p->defensa,
			    p->velocidad) < 0) {
			fclose(f);
			return NULL;
		}
	}

	fclose(f);
	return tp1;
}

//OPERACIONES ENTRE TPS
//Unión
tp1_t *tp1_union(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = tp1_crear();
	if (!nuevo)
		return NULL;

	size_t i = 0, j = 0;
	while (i < un_tp->cantidad || j < otro_tp->cantidad) {
		struct pokemon *p = NULL;

		if (i < un_tp->cantidad &&
		    (j >= otro_tp->cantidad ||
		     un_tp->pokemones[i].id < otro_tp->pokemones[j].id)) {
			p = &un_tp->pokemones[i++];
		} else if (j < otro_tp->cantidad &&
			   (i >= un_tp->cantidad ||
			    otro_tp->pokemones[j].id <
				    un_tp->pokemones[i].id)) {
			p = &otro_tp->pokemones[j++];
		} else {
			p = &un_tp->pokemones[i++];
			j++;
		}

		struct pokemon copia = copiar_pokemon(p);

		if (!tp1_agregar_pokemon(nuevo, copia)) {
			free(copia.nombre);
			tp1_destruir(nuevo);
			return NULL;
		}
	}

	return nuevo;
}

//Intersección
tp1_t *tp1_interseccion(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = tp1_crear();
	if (!nuevo)
		return NULL;

	size_t i = 0, j = 0;
	while (i < un_tp->cantidad && j < otro_tp->cantidad) {
		if (un_tp->pokemones[i].id == otro_tp->pokemones[j].id) {
			struct pokemon copia =
				copiar_pokemon(&un_tp->pokemones[i]);
			if (!tp1_agregar_pokemon(nuevo, copia)) {
				free(copia.nombre);
				tp1_destruir(nuevo);
				return NULL;
			}
			i++;
			j++;
		} else if (un_tp->pokemones[i].id < otro_tp->pokemones[j].id) {
			i++;
		} else {
			j++;
		}
	}
	return nuevo;
}

//Diferencia
tp1_t *tp1_diferencia(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *nuevo = tp1_crear();
	if (!nuevo)
		return NULL;

	size_t i = 0, j = 0;

	while (i < un_tp->cantidad) {
		int id1 = un_tp->pokemones[i].id;
		int id2 = (j < otro_tp->cantidad) ? otro_tp->pokemones[j].id :
						    -1;

		if (j >= otro_tp->cantidad || id1 < id2) {
			struct pokemon p = un_tp->pokemones[i];
			p.nombre = duplicar_cadena(un_tp->pokemones[i].nombre);
			if (!p.nombre || !tp1_agregar_pokemon(nuevo, p)) {
				if (p.nombre)
					free(p.nombre);
				tp1_destruir(nuevo);
				return NULL;
			}
			i++;
		} else if (id1 == id2) {
			i++;
			j++;
		} else {
			j++;
		}
	}

	return nuevo;
}
