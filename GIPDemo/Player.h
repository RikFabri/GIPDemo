#pragma once
#include "World.h"
#include "VectorMath.h"
#include "Inventory.h"
#include "InputManager.h"
#if _DEBUG
#include "allegro5/allegro_primitives.h"
#endif

class Player
	: public IAnimation
{
private:
	ALLEGRO_BITMAP *texture;
	Vector movement;
	float speed;
	int flag = 0;
public:
	World *world;
	float width, height;
	float x, y;
	void update();
	void updatePhysics();
	void render(Camera *c);
	void placeBlock(ALLEGRO_EVENT event, Camera *c, Inventory *inventory);
	void breakBlock(ALLEGRO_EVENT event, Camera *c, Inventory *inventory);
	Player();
	~Player();
};