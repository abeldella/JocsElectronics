#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "entity.h"

class Controller
{
public:
	Entity* target;
	const Uint8* keystate;
	SDL_Joystick* pad;
	Camera* camera;
	bool active;

	Controller();
	virtual ~Controller();
	void update(float dt);

	Camera* getCamera();
};

#endif
