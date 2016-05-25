#include "controller.h"
#include "includes.h"
#include "game.h"
#include "framework.h"

Controller::Controller()
{
	target = NULL;
	pad = NULL;
	camera = NULL;
	keystate = NULL;
	/*mouse_locked = false;*/
	active = true;
}
Controller::~Controller()
{

}

void Controller::update(float dt)
{
	Fighter* player = (Fighter*)target;
	Game* game = Game::instance;
	keystate = game->keystate;

	if (active) {

		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) player->rotate(-90 * dt, Vector3(1, 0, 0));
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) player->rotate(90 * dt, Vector3(1, 0, 0));
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) player->rotate(-90 * dt, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) player->rotate(90 * dt, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_Q]) player->rotate(90 * dt * 0.5, Vector3(0, 0, -1));
		if (keystate[SDL_SCANCODE_E]) player->rotate(90 * dt * 0.5, Vector3(0, 0, 1));

		if ((game->mouse_state & SDL_BUTTON_LEFT) || game->mouse_locked) //is left button pressed?
		{
			player->rotate(game->mouse_delta.x * 0.03, Vector3(0, 0, 1));
			player->rotate(game->mouse_delta.y * -0.03, Vector3(1, 0, 0));
		}

		//ACTIONS
		if (keystate[SDL_SCANCODE_F]) player->shoot();
		if (keystate[SDL_SCANCODE_SPACE]) player->accelerate();
	}
	//WITH XBOX CONTROLLER
	if (pad) {
		JoystickState pad_state = getJoystickState(pad);

		if (abs(pad_state.axis[LEFT_ANALOG_Y]) > 0.1)
			player->rotate(90 * pad_state.axis[LEFT_ANALOG_Y] * dt, Vector3(1, 0, 0));
		if (abs(pad_state.axis[LEFT_ANALOG_X]) > 0.1)
			player->rotate(-90 * pad_state.axis[LEFT_ANALOG_X] * dt, Vector3(0, 0, 1));

		if (abs(pad_state.axis[RIGHT_ANALOG_X]) > 0.1) 
			player->camera_info.x = PI/2 * pad_state.axis[RIGHT_ANALOG_X];
		else
			player->camera_info.x = 0;

		if (abs(pad_state.axis[RIGHT_ANALOG_Y]) > 0.1) {
			player->camera_info.z = pad_state.axis[RIGHT_ANALOG_Y];
		}
		//else player->camera_info.z = 0;

		if (pad_state.button[RIGHT_ANALOG_BUTTON])
		{
			std::cout << "RAB shooting " << pad_state.button[RIGHT_ANALOG_BUTTON] << std::endl;
			player->shoot();
		}

		if (pad_state.button[BACK_BUTTON]) exit(0);
		if (pad_state.button[Y_BUTTON]) {
			World* world = World::getInstance();
			world->boss->destroyEntity();
		}
		if (pad_state.button[RB_BUTTON]) {
			player->accelerate();			
		}
				
		
		/*CONTROL DE TRIGGERS
		left = +max( 0.0f, pad_state.button[TRIGGERS] );
		right = -max( 0.0f, pad_state.button[TRIGGERS] );
		*/
	}

	Matrix44 global_player_matrix = player->getGlobalMatrix();
	//camera->lookAt(global_player_matrix * Vector3(0, 2, -5), global_player_matrix *  Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));
	if(!player->accelerator)
		camera = getCamera();
	else {
		for (int i = -5; i <= -25; i++) {
			camera->lookAt(global_player_matrix * Vector3(0, 2, i), global_player_matrix *  Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));
		}
		camera->lookAt(global_player_matrix * Vector3(0, 2, -25), global_player_matrix *  Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));
	}
}

Camera* Controller::getCamera()
{
	Fighter* fighter = (Fighter*)target;
	fighter->updateCamera(camera);
	return camera;
}