#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include <iostream>
#include "game.h"

int main()
{
	al_init();
	al_init_image_addon();

	game g;
	g.init();

	return 0;
}