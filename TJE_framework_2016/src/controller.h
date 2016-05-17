#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "entity.h"

class Controller
{
private:
	Entity* target;
	const Uint8* keystate;

	int mouse_state;
	Vector2 mouse_delta;
	bool mouse_locked;

public:
	Controller();
	void update(double dt);
};

#endif
