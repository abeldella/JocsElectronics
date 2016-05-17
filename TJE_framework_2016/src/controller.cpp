#include "controller.h"
#include "includes.h"
#include "game.h"
#include "framework.h"

Controller::Controller()
{
	keystate = NULL;
	mouse_locked = false;
}

void Controller::update(double dt)
{

	Camera* current_camera = Game::instance->current_camera;
	Camera* free_camera = Game::instance->free_camera;
	Camera* player_camera = Game::instance->player_camera;
	double speed = dt * 100;
	target = Game::instance->player;

	if (current_camera == free_camera) {

		//mouse input to rotate the cam
		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			current_camera->rotate(mouse_delta.x * 0.005, Vector3(0, -1, 0));
			current_camera->rotate(mouse_delta.y * 0.005, current_camera->getLocalVector(Vector3(-1, 0, 0)));
		}

		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) current_camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) current_camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) current_camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) current_camera->move(Vector3(-1, 0, 0) * speed);

		//to navigate with the mouse fixed in the middle
	}
	else if (current_camera == player_camera) {


		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) target->rotate(-90 * dt, Vector3(1, 0, 0));
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) target->rotate(90 * dt, Vector3(1, 0, 0));
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) target->rotate(-90 * dt, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) target->rotate(90 * dt, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_Q]) target->rotate(90 * dt * 0.5, Vector3(0, 0, -1));
		if (keystate[SDL_SCANCODE_E]) target->rotate(90 * dt * 0.5, Vector3(0, 0, 1));


		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			target->rotate(mouse_delta.x * 0.03, Vector3(0, 0, 1));
			target->rotate(mouse_delta.y * -0.03, Vector3(1, 0, 0));
		}

		if (keystate[SDL_SCANCODE_F]) ((Fighter*)target)->shoot();


	}


}