#include <stdlib.h>

#include "graph.h"

#define MAP_SIZE 32
#define CELL_SIZE 32

typedef struct
{
	RUS_BITMAP *floor;
	RUS_BITMAP *stone;
} TEXTURES;

void load_map(int Map[][MAP_SIZE], char *filename);

void draw_map(int Map[][MAP_SIZE], int map_shift_x, int map_shift_y);