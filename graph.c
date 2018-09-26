#include "graph.h"


void rus_init_graph()
{
	al_init();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
}


RUS_DISPLAY *rus_create_display(int width, int height)
{
	ALLEGRO_DISPLAY *al_disp = al_create_display(width, height);
	RUS_DISPLAY *display = malloc(sizeof(RUS_DISPLAY));
	display->display = al_disp;
	return display;
}


RUS_BITMAP *rus_load_bitmap(char filename[])
{
	RUS_BITMAP *bitmap = malloc(sizeof(RUS_BITMAP));
	bitmap->bitmap = al_load_bitmap(filename);
	return bitmap;
}


void rus_draw_bitmap(RUS_BITMAP *bitmap, int x, int y)
{
	al_draw_bitmap(bitmap->bitmap, x, y, 0);
}


void rus_get_keyboard_state(RUS_KEYBOARD_STATE *keyboard)
{
	ALLEGRO_KEYBOARD_STATE *state = malloc(sizeof(ALLEGRO_KEYBOARD_STATE));
	al_get_keyboard_state(state);
	
	keyboard->keyboard = state;
	free(state);
}


bool rus_key_down(RUS_KEYBOARD_STATE *keyboard, int keycode)
{
	al_key_down(keyboard->keyboard, keycode);
}


void rus_clear_to_color(RUS_COLOR color)
{
	al_clear_to_color(color.color);
}

RUS_COLOR rus_map_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	RUS_COLOR color = { al_map_rgb(r, g, b) };
	return color;
}


void rus_flip_display()
{
	al_flip_display();
}


void rus_destroy_display(RUS_DISPLAY *display)
{
	al_destroy_display(display->display);
}