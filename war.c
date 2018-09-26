#include <string.h>
#include <time.h>

#include "map.h"


bool user_exit = false;
int map_shift_x = -CELL_SIZE, map_shift_y = -CELL_SIZE;


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
	Direction direction;
	float speed;
	RUS_BITMAP *bitmap;
	struct unit_type *next;
} unit;


unit *create_unit(Type type)
{
	unit *peon = malloc(sizeof(type));
	
	peon->type = type;
	peon->state = STAY;
	peon->x = rand() % WIDTH;
	peon->y = rand() % HEIGHT;
	peon->frame = 0;
	peon->direction = S;
	peon->speed = 1.0;
	peon->bitmap = rus_load_bitmap("images\\peon.png");
	
	return peon;	
}


int main(void)
{
	rus_init_graph();
	int Map[MAP_SIZE][MAP_SIZE]; 
	load_map(Map, "Map.txt");
	RUS_DISPLAY *display = NULL;
	//al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	display = rus_create_display(WIDTH, HEIGHT);
	//al_set_window_title(display, "War Craft 2 Clone");
	RUS_KEYBOARD_STATE *keyboard = malloc(sizeof(RUS_KEYBOARD_STATE));
	/*unit *peon = create_unit(PEON);
	unit *i = peon;
	for (int n = 1; n < 10; n++)
	{
		unit *new_peon = create_unit(PEON);
		i->next = new_peon;
		i = new_peon;
	}*/
	while (!user_exit)
	{
		rus_get_keyboard_state(keyboard);
		if (rus_key_down(keyboard, RUS_KEY_ESCAPE ) ) user_exit = true;
        if (rus_key_down(keyboard, RUS_KEY_DOWN ) ) map_shift_y -= 1;
		if (rus_key_down(keyboard, RUS_KEY_UP ) ) map_shift_y += 1;
		if (rus_key_down(keyboard, RUS_KEY_RIGHT ) ) map_shift_x -= 1;
		if (rus_key_down(keyboard, RUS_KEY_LEFT ) ) map_shift_x += 1;

		rus_clear_to_color(rus_map_rgb(0, 0, 0));
		draw_map(Map, map_shift_x, map_shift_y);
		/*unit *j = peon;
		unit *next = NULL;
		for (int n = 0; n < 10; n++)
		{
			rus_draw_bitmap(j->bitmap, j->x + map_shift_x, j->y + map_shift_y);
			j = j->next;
		}*/
		
		
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

