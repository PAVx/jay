#include "internal_map_create.h"
#include <stdio.h>
#include <stdlib.h>

static double _vessicle_size = -1.0;

int main (void) {
	uint64_t rows = 8;
	uint64_t col = 8;
	uint64_t count = 0;
	map_t *map = map_create(rows, col);

	map_print(map, stdout);

	for (uint64_t i = 0; i < rows; i++) {
		for (uint64_t j = 0; j < col; j++) {
			map_add_value(map, i, j, 0);
		}
	}

	map_add_value(map, 1, 1, 10);
	map_print(map, stdout);

	return 0;
}


// For a map creation, the size of each scannable vessicle must be known
void store_vessicle_size(double size) {
	if (size < 0.0) {
		return;
	} 

	_vessicle_size = size;
}

double get_vessicle_size(void) {
	return _vessicle_size;
}

map_t *map_create(double rows, double columns) {
	map_t *new_map = NULL;
	uint64_t itor_i = 0;
	uint64_t itor_j = 0;

	new_map = malloc(sizeof(map_t));
	if (new_map) {

		// allocating "rows" number of map rows
		new_map->map = malloc(sizeof(struct vessicle) * rows);

		for (itor_i = 0; itor_i < rows; itor_i++) {
			// allocating jth (j <= columns) map columns
			new_map->map[itor_i] = malloc(sizeof(struct vessicle) * columns);

			if(new_map->map[itor_i]) {
				for (itor_j = 0; itor_j < columns; itor_j++) {
					// initializing each vessicle
					new_map->map[itor_i][itor_j].value = -1;
					#ifdef STORE_COORDINATES
						new_map->map[itor_i][itor_j].longitude = -1;
						new_map->map[itor_i][itor_j].latitude = -1;
					#endif
				}
			}
		}

		new_map->rows = rows;
		new_map->columns = columns;
	}

	return new_map;
}

void map_destroy(map_t *map) {
	uint64_t itor_i = 0;
	uint64_t itor_j = 0;

	if (map) {
		// allocating "rows" number of map rows
		for (itor_i = 0; itor_i < map->rows; itor_i++) {
			if(map->map[itor_i]) {
				for (itor_j = 0; itor_j < map->columns; itor_j++) {
					if(&(map->map[itor_i][itor_j])) {
						free(&(map->map[itor_i][itor_j]));
				}
				free(map->map[itor_i]);
			}
		}
	}
	free(map->map);
	}

	free(map);
	map = NULL;
}

int8_t map_add_value(map_t *map, uint64_t row, uint64_t column, double value) {
	if (map) {
		if(map->map) {
			if(map->map[row]) {
				if(&(map->map[row][column])) {
					map->map[row][column].value = value;

					return MAP_SUCCESS;
				}
			}
		}
	}

	return MAP_ERROR;
}

#ifdef MAP_DEBUG
	void map_print(map_t *map, FILE *stream) {
		uint64_t itor_i = 0;
		uint64_t itor_j = 0;

		fprintf(stream, "\nPRINTING %lldX%lld MAP\n\n", map->rows, map->columns);

		if (map) {
			// allocating "rows" number of map rows
			for (itor_i = 0; itor_i < map->rows; itor_i++) {
				if(map->map[itor_i]) {
					for (itor_j = 0; itor_j < map->columns; itor_j++) {
						if(map->map[itor_j]) {
							fprintf(stream, "[%0.2lf]\t", map->map[itor_i][itor_j].value);
						}
					}
				fprintf(stream, "\n");
				}
			}

			fprintf(stdout, "******************\n");
		}
	}
#endif

double map_euclidean_distance(map_t *map, uint64_t row_a, uint64_t col_a, uint64_t row_b, uint64_t col_b) {
	if(map) {
		if(map->map) {
			if(map->map[col_a][col_b])
		}
	}
}
double map_predict_value(map_t *map, uint64_t row, uint64_t col);