#ifndef _INTERNAL_MAP_CREATE_H_
#define _INTERNAL_MAP_CREATE_H_

#include <stdint.h>
#include <stdio.h>

//#define STORE_COORDINATES (1)
#define MAP_DEBUG (1)

#define MAP_SUCCESS (1)
#define MAP_ERROR (-1)

struct vessicle {
	double value;
#ifdef STORE_COORDINATES
	double longitude;
	double latitude;
#endif
};

struct map {
	uint64_t rows;
	uint64_t columns;
	struct vessicle **map;
};
typedef struct map map_t;

// For a map creation, the size of each scannable vessicle must be known
void store_vessicle_size(double size);
double get_vessicle_size(void);
double map_calculate_vessicle_rows(double vessicle_size, double point_a, double point_b);
double map_calculate_vessicle_columns(double vessicle_size, double point_a, double point_b);

map_t *map_create(double rows, double columns);
void map_destroy(map_t *map);

#ifdef MAP_DEBUG
	void map_print(map_t *map, FILE *stream);
#endif

int8_t map_add_value(map_t *map, uint64_t row, uint64_t column, double value);
double map_euclidean_distance(map_t *map, uint64_t row_a, uint64_t col_a, uint64_t row_b, uint64_t col_b);
double map_predict_value(map_t *map, uint64_t row, uint64_t col);

#endif // _INTERNAL_MAP_CREATE_H_