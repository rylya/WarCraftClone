#include <string.h>
#include <time.h>
#include <math.h>

#include "map.h"


bool user_exit = false;
int map_shift_x = 0, map_shift_y = 0;


typedef enum
{
	PEON,
	WARRIOR,
	ARCHER
} Type;

typedef enum
{
	STAY,
	GO,
	ATTACK,
	DEAD
} State;

typedef enum
{
	N, NE, E, SE, S, SW, W, NW
} Direction;

typedef struct unit_type
{
	Type type;
	State state;
	unsigned int x:10, y:9;
	unsigned int frame:3;
	unsigned int dest_x:10, dest_y:9;
	Direction direction;
	float speed;
	RUS_BITMAP *bitmap;
	struct unit_type *next;
} unit;


unit *create_unit(Type type)
{
	unit *peon = malloc(sizeof(unit));
	
	peon->type = type;
	peon->state = STAY;
	peon->x = rand() % WIDTH;
	peon->y = rand() % HEIGHT;
	peon->frame = 0;
	peon->direction = S;
	peon->speed = 1.0;
	peon->bitmap = rus_load_bitmap("images\\peon.png");
	peon->dest_x = peon->x;
	peon->dest_y = peon->y;
	
	return peon;	
}


int main(void)
{
	rus_init_graph();

	if (!al_install_mouse())
	{
		puts("Error installing mouse");
		return -1;
	}
	
	//set_mouse_sprite(al_load_bitmap("images\\new\\mouse.png"));
		
	int Map[MAP_SIZE][MAP_SIZE]; 
	load_map(Map, "Map.txt");

	RUS_DISPLAY *display = NULL;
	//rus_set_display_flags(RUS_FULLSCREEN);
	display = rus_create_display(WIDTH, HEIGHT);
	rus_set_window_title(display, "War Craft 2 Clone");
	RUS_KEYBOARD_STATE *keyboard = malloc(sizeof(RUS_KEYBOARD_STATE));

	unit *peon = create_unit(PEON);

	/*unit *i = peon;
	for (int n = 1; n < 10; n++)
	{
		unit *new_peon = create_unit(PEON);
		i->next = new_peon;
		i = new_peon;
	}*/
	
	ALLEGRO_MOUSE_STATE state;
	
	while (!user_exit)
	{
		///////////////////input/////////////////////////////////////////////////////
		rus_get_keyboard_state(keyboard);
		if (rus_key_down(keyboard, RUS_KEY_ESCAPE ) ) user_exit = true;
        if (rus_key_down(keyboard, RUS_KEY_DOWN ) ) map_shift_y -= 1;
		if (rus_key_down(keyboard, RUS_KEY_UP ) ) map_shift_y += 1;
		if (rus_key_down(keyboard, RUS_KEY_RIGHT ) ) map_shift_x -= 1;
		if (rus_key_down(keyboard, RUS_KEY_LEFT ) ) map_shift_x += 1;

		al_get_mouse_state(&state);
		if (state.buttons & 1)
		{
			printf("Left pressed. Mouse position: (%d, %d)\n", state.x, state.y);
		}
		if (state.buttons & 2)
		{
			peon->dest_x = (lround((state.x - map_shift_x) / CELL_SIZE)) * CELL_SIZE;
			peon->dest_y = (lround((state.y - map_shift_y) / CELL_SIZE)) * CELL_SIZE;
			printf("Right pressed. Mouse position: (%d, %d)\n", state.x, state.y);
			printf("Right pressed. Mouse position: (%d, %d)\n", lround((float)state.x / CELL_SIZE), lround((float)state.y / CELL_SIZE));
			printf("Right pressed. Corrected mouse position: (%d, %d)\n", peon->dest_x, peon->dest_y);
		}
		
		////////////////unit logic/////////////////////////////////////////////////////
		
		int x_direction = 0;
		if (peon->x < peon->dest_x) { peon->x += peon->speed; x_direction++; }
		if (peon->x > peon->dest_x) { peon->x -= peon->speed; x_direction--; }
		
		int y_direction = 0;
		if (peon->y < peon->dest_y) { peon->y += peon->speed; y_direction++; }
		if (peon->y > peon->dest_y) { peon->y -= peon->speed; y_direction--; }
		
		/*if (x_direction > 0 && y_direction == 0) peon->direction = E;
		if (x_direction > 0 && y_direction > 0) peon->direction = SE;
		if (x_direction 
		*/
		/////////////////drawing///////////////////////////////////////////////////////
		rus_clear_to_color(rus_map_rgb(0, 0, 0));
		draw_map(Map, map_shift_x, map_shift_y);

		rus_draw_bitmap(peon->bitmap, peon->x + map_shift_x, peon->y + map_shift_y);
		
		//unit *j = peon;
		//unit *next = NULL;
		//for (int n = 0; n < 10; n++)
		//{
		//	rus_draw_bitmap(j->bitmap, j->x + map_shift_x, j->y + map_shift_y);
		//	j = j->next;
		//}
		
		rus_flip_display();
	}
	
	rus_destroy_display(display);

	return 0;
}


//gcc opengl.c -o opengl -lallegro-5.0.10-monolith-md
	//al_draw_bitmap(bitmap, 20, 20, 20);
	//ALLEGRO_BITMAP *bitmap = al_load_bitmap("d:\\i.jpg");
	//al_rest(3);
		//al_draw_circle(x, y, 20, al_map_rgb(255, 0, 0), 1.0f);

