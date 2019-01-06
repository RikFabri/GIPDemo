#include "Inventory.h"



void Inventory::loadTypes(std::string path, Textures *t, World *world)
{
	tinyxml2::XMLDocument XMLDoc;
	tinyxml2::XMLError eResult = XMLDoc.LoadFile(path.c_str());
	if (eResult != tinyxml2::XML_SUCCESS) { std::cout << "Inventory-object types xml failed to load."; };

	tinyxml2::XMLElement *Root = XMLDoc.FirstChildElement("InventoryObjectTypes");

	for (tinyxml2::XMLElement* child = Root->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
		std::string name = child->Attribute("name");
		std::string texture = child->Attribute("texture");
		InventoryObject iObj = InventoryObject(t->textures[texture], &world->terrainTypes[name]);
		inventoryObjects[name] = iObj;
	}
	for (auto it : inventoryObjects) {
		std::cout << it.first << ": ";
		std::cout << it.second.texture << std::endl;
	}
}

void Inventory::addObject(InventoryObject * io)
{
}

void Inventory::selectFrame(int dIndex)
{
	selectedFrame += dIndex;
	if (selectedFrame < 0)
		selectedFrame = 5;
	if (selectedFrame > 5)
		selectedFrame = 0;
}

void Inventory::init(Textures *t, World *world)
{
	loadTypes("Data/inventory.xml", t, world);
	this->frame = t->textures["frame"];
	this->selectionFrame = al_load_bitmap("Images/selectionFrame.png");
	PausedEvents_Queue = al_create_event_queue();
	UnpausedEvents_Queue = al_create_event_queue();
	al_register_event_source(PausedEvents_Queue, al_get_mouse_event_source());
	al_register_event_source(UnpausedEvents_Queue, al_get_mouse_event_source());
	selectedFrame = 1;
	font = al_load_font("Data/font.otf", 14, 0);
	this->inventoryArray[0][0] = { &inventoryObjects["grassBlock"], 3 };
	this->inventoryArray[0][2] = { &inventoryObjects["cobbleBlock"], 1 };
	this->inventoryArray[1][2] = { &inventoryObjects["dirtBlock"], 6 };
	this->inventoryArray[1][3] = { &inventoryObjects["woodBlock"], 6 };
	this->inventoryArray[1][4] = { &inventoryObjects["woodBlock"], 1 };

}

void Inventory::setPaused(bool b)
{
	al_pause_event_queue(PausedEvents_Queue, b);
}

bool Inventory::holdingSomething()
{
	if (inventoryArray[selectedFrame][0].first)
		return true;
	return false;
}

Terrain * Inventory::getHand()
{
	return this->inventoryArray[selectedFrame][0].first->terrainPointer;
}

void Inventory::DragItem(float x, float y, std::string mode)
{
	for (int i = 0; i < FRAMES_WIDTH; i++) {
		for (int j = 0; j < FRAMES_HEIGHT; j++) {
			if (x >= i * SCALE + XOFFSET && x < (i + 1)*SCALE + XOFFSET &&
				y >= j * SCALE + YOFFSET && y < (j + 1)*SCALE + YOFFSET) {

				if (mode == "select" && selectedObj.first == NULL) {
					this->selectedObj = { inventoryArray[i][j].first, inventoryArray[i][j].second };
					inventoryArray[i][j].first = NULL;
				}
				else {
					if (mode == "drop" && selectedObj.first != NULL) {
						//Copy possible objects from drop location
						std::pair<InventoryObject*, int>tmp = inventoryArray[i][j];
						//Drop dragged object
						inventoryArray[i][j] = { selectedObj.first, selectedObj.second };
						//if container contained object
						if (tmp.first != NULL) {
							//Add amount if types are the same
							if (selectedObj.first == tmp.first) {
								inventoryArray[i][j].second += tmp.second;
								tmp = { NULL, NULL };
							}
						//Remove or switch selected object
						}selectedObj = tmp;
					}
				}
			}
		}
	}
}


void Inventory::handlePausedEvents()
{
	ALLEGRO_EVENT E;
	while (al_get_next_event(PausedEvents_Queue, &E)) {
		switch (E.type)
		{
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			DragItem(E.mouse.x, E.mouse.y, "select");
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			DragItem(E.mouse.x, E.mouse.y, "drop");
			break;
		}
	}
}

void Inventory::handleUnpausedEvents()
{
	ALLEGRO_EVENT E;
	while (al_get_next_event(UnpausedEvents_Queue, &E)) {
		switch (E.type)
		{
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (E.mouse.dz != 0)
				selectFrame(E.mouse.dz);
			break;
		}
	}
}

void Inventory::drawHotbar()
{
	for (int i = 0; i < FRAMES_WIDTH; i++) {
		int j = 0;
		al_draw_bitmap(frame, i * SCALE + XOFFSET, j * SCALE + YOFFSET, 0);
		if (inventoryArray[i][j].first != NULL) {
			al_draw_bitmap(inventoryArray[i][j].first->texture, i * SCALE + XOFFSET + XOFFSET_OBJ, j * SCALE + YOFFSET + YOFFSET_OBJ, 0);
			al_draw_text(font, al_map_rgb(255, 255, 255), i * SCALE + XOFFSET + XOFFSET_OBJ, j * SCALE + YOFFSET + YOFFSET_OBJ, 0, std::to_string(inventoryArray[i][j].second).c_str());
		}
	}
	al_draw_bitmap(selectionFrame, selectedFrame * SCALE, 0, 0);
}

void Inventory::draw()
{
	for (int i = 0; i < FRAMES_WIDTH; i++) {
		for (int j = 0; j < FRAMES_HEIGHT; j++) {
			al_draw_bitmap(frame, i * SCALE + XOFFSET, j * SCALE + YOFFSET, 0);
			if (inventoryArray[i][j].first != NULL) {
				al_draw_bitmap(inventoryArray[i][j].first->texture, i * SCALE + XOFFSET + XOFFSET_OBJ, j * SCALE + YOFFSET + YOFFSET_OBJ, 0);
				al_draw_text(font, al_map_rgb(255, 255, 255), i * SCALE + XOFFSET + XOFFSET_OBJ, j * SCALE + YOFFSET + YOFFSET_OBJ, 0, std::to_string(inventoryArray[i][j].second).c_str());
			}
		}
	}
	if (selectedObj.first != NULL && selectedObj.second != NULL) {
		int x, y;
		al_get_mouse_cursor_position(&x, &y);
		x -= 16;
		y -= 16;
		al_draw_bitmap(selectedObj.first->texture, x, y, 0);
		al_draw_text(font, al_map_rgb(255, 255, 255), x, y, 0, std::to_string(selectedObj.second).c_str());
	}
	al_draw_bitmap(selectionFrame, selectedFrame * SCALE, 0, 0);
}

void Inventory::update()
{
	handlePausedEvents();
	handleUnpausedEvents();
}

Inventory::Inventory()
{
}


Inventory::~Inventory()
{
}
