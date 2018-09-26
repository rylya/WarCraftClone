#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <stdio.h>

#define RUS_KEY_ESCAPE ALLEGRO_KEY_ESCAPE
#define RUS_KEY_DOWN ALLEGRO_KEY_DOWN
#define RUS_KEY_UP ALLEGRO_KEY_UP
#define RUS_KEY_RIGHT ALLEGRO_KEY_RIGHT
#define RUS_KEY_LEFT ALLEGRO_KEY_LEFT

#define WIDTH 640
#define HEIGHT 480

typedef struct
{
	ALLEGRO_BITMAP *bitmap;
} RUS_BITMAP;


typedef struct
{
	ALLEGRO_DISPLAY *display;
} RUS_DISPLAY;


typedef struct
{
	ALLEGRO_KEYBOARD_STATE *keyboard;
} RUS_KEYBOARD_STATE;


typedef struct
{
	ALLEGRO_COLOR color;
} RUS_COLOR;


void rus_init_graph();
RUS_DISPLAY *rus_create_display(int width, int height);
RUS_BITMAP *rus_load_bitmap(char filename[]);
void rus_draw_bitmap(RUS_BITMAP *bitmap, int x, int y);
void rus_get_keyboard_state(RUS_KEYBOARD_STATE *keyboard);
bool rus_key_down(RUS_KEYBOARD_STATE *keyboard, int keycode);
void rus_clear_to_color(RUS_COLOR color);
RUS_COLOR rus_map_rgb(unsigned char r, unsigned char g, unsigned char b);
void rus_flip_display();
void rus_destroy_display(RUS_DISPLAY *display);