#include "map.h"


void load_map(int Map[][MAP_SIZE], char *filename)
{
	FILE *f = fopen(filename, "r");
	
	for (int j = 0; j < MAP_SIZE; j++)
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			char symbol = fgetc(f);
			if (symbol == '\n' || symbol == ' ')
				symbol = fgetc(f);

			Map[i][j] = atoi(&symbol);
		}
	}
	
	fclose(f);
}

void draw_map(int Map[][MAP_SIZE], int map_shift_x, int map_shift_y)
{
	RUS_BITMAP *floor = rus_load_bitmap("images\\new\\grass.png");
	RUS_BITMAP *farm1 = rus_load_bitmap("images\\new\\farm1.png");
	RUS_BITMAP *farm2 = rus_load_bitmap("images\\new\\farm2.png");
	RUS_BITMAP *farm3 = rus_load_bitmap("images\\new\\farm3.png");
	RUS_BITMAP *farm4 = rus_load_bitmap("images\\new\\farm4.png");
	RUS_BITMAP *tree = rus_load_bitmap("images\\new\\tree.png");
	
	for (int j = 0; j < MAP_SIZE; j++)
	{	
		for (int i = 0; i < MAP_SIZE; i++)
		{
			int cell_x = i * CELL_SIZE + map_shift_x;
			int cell_y = j * CELL_SIZE + map_shift_y;
				
			if ((cell_x >= 0 && cell_x < WIDTH - CELL_SIZE) &&
				(cell_y >= 0 && cell_y < HEIGHT - CELL_SIZE))
				{
					switch (Map[i][j])
					{
						case 0: rus_draw_bitmap(floor, cell_x, cell_y); break;
						case 1: rus_draw_bitmap(farm1, cell_x, cell_y); break;
						case 2: rus_draw_bitmap(farm2, cell_x, cell_y); break;
						case 3: rus_draw_bitmap(farm3, cell_x, cell_y); break;
						case 4: rus_draw_bitmap(farm4, cell_x, cell_y); break;
						case 9: rus_draw_bitmap(tree, cell_x, cell_y); break;
					}
				}
		}
	}
	
	free(floor);
	free(farm1);
	free(farm2);
	free(farm3);
	free(farm4);
	free(tree);
}