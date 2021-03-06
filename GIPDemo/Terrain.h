#pragma once
#include "Textures.h"

class Terrain
{
private:
	ALLEGRO_BITMAP *texture;
	int power;
	bool solid;
public:
	std::string name;
	float zoom;
	ALLEGRO_BITMAP* getTexture();
	int getPower();
	bool isSolid();
	Terrain(ALLEGRO_BITMAP *texture, int power, bool solid, std::string name);
	Terrain();
	~Terrain();
};

