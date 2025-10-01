#include "src/tp1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mostrar_pokemon(struct pokemon *p)
{
	if (!p)
		return;
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
		tipo_str = "DESCONOCIDO";
		break;
	}
	printf("ID: %d | Nombre: %s | Tipo: %s | Ataque: %d | Defensa: %d | Velocidad: %d\n",
	       p->id, p->nombre, tipo_str, p->ataque, p->defensa, p->velocidad);
}

typedef struct {
	struct pokemon **array;
	size_t i;
} aux_array;

bool callback_guardar(struct pokemon *p, void *extra)
{
	aux_array *a = (aux_array *)extra;
	a->array[a->i++] = p;
	return true;
}

int cmp_nombre(const void *a, const void *b)
{
	struct pokemon *pa = *(struct pokemon **)a;
	struct pokemon *pb = *(struct pokemon **)b;
	return strcmp(pa->nombre, pb->nombre);
}

int cmp_id(const void *a, const void *b)
{
	struct pokemon *pa = *(struct pokemon **)a;
	struct pokemon *pb = *(struct pokemon **)b;
	return pa->id - pb->id;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Uso incorrecto\n");
		return 1;
	}

	const char *archivo1 = argv[1];
	tp1_t *tp = tp1_leer_archivo(archivo1);
	if (!tp) {
		fprintf(stderr, "No se pudo leer %s\n", archivo1);
		return 1;
	}

	if (strcmp(argv[2], "buscar") == 0) {
		if (argc < 5) {
			fprintf(stderr, "Uso: %s buscar <nombre|id> <valor>\n",
				argv[0]);
			tp1_destruir(tp);
			return 1;
		}
		if (strcmp(argv[3], "nombre") == 0) {
			struct pokemon *p = tp1_buscar_nombre(tp, argv[4]);
			if (p)
				mostrar_pokemon(p);
			else
				printf("No encontrado %s\n", argv[4]);
		} else if (strcmp(argv[3], "id") == 0) {
			int id = atoi(argv[4]);
			struct pokemon *p = tp1_buscar_id(tp, id);
			if (p)
				mostrar_pokemon(p);
			else
				printf("No encontrado ID %d\n", id);
		}
	} else if (strcmp(argv[2], "union") == 0 ||
		   strcmp(argv[2], "interseccion") == 0 ||
		   strcmp(argv[2], "diferencia") == 0) {
		if (argc < 5) {
			fprintf(stderr,
				"Uso: %s %s <archivo2.csv> <resultado.csv>\n",
				argv[0], argv[2]);
			tp1_destruir(tp);
			return 1;
		}

		tp1_t *tp2 = tp1_leer_archivo(argv[3]);
		if (!tp2) {
			fprintf(stderr, "No se pudo leer %s\n", argv[3]);
			tp1_destruir(tp);
			return 1;
		}

		tp1_t *res = NULL;
		if (strcmp(argv[2], "union") == 0)
			res = tp1_union(tp, tp2);
		else if (strcmp(argv[2], "interseccion") == 0)
			res = tp1_interseccion(tp, tp2);
		else if (strcmp(argv[2], "diferencia") == 0)
			res = tp1_diferencia(tp, tp2);

		if (res) {
			if (!tp1_guardar_archivo(res, argv[4]))
				fprintf(stderr, "Error guardando en %s\n",
					argv[4]);
			else
				printf("Resultado guardado en %s\n", argv[4]);
			tp1_destruir(res);
		}

		tp1_destruir(tp2);
	} else if (strcmp(argv[2], "mostrar") == 0) {
		if (argc < 4) {
			fprintf(stderr, "Uso: %s mostrar <nombre|id>\n",
				argv[0]);
			tp1_destruir(tp);
			return 1;
		}

		size_t n = tp1_cantidad(tp);
		struct pokemon **array = malloc(sizeof(struct pokemon *) * n);
		aux_array aux = { array, 0 };
		tp1_con_cada_pokemon(tp, callback_guardar, &aux);

		if (strcmp(argv[3], "nombre") == 0)
			qsort(array, n, sizeof(struct pokemon *), cmp_nombre);
		else if (strcmp(argv[3], "id") == 0)
			qsort(array, n, sizeof(struct pokemon *), cmp_id);

		for (size_t i = 0; i < n; i++)
			mostrar_pokemon(array[i]);
		free(array);
	} else {
		fprintf(stderr, "Operación desconocida: %s\n", argv[2]);
	}

	tp1_destruir(tp);
	return 0;
}
