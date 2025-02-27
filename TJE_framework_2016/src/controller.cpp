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
		//else
			//player->camera_info.z = 0;
		
		if (pad_state.button[RIGHT_ANALOG_BUTTON]) player->rotate(30 * dt, Vector3(0, 1, 0));
		if (pad_state.button[LEFT_ANALOG_BUTTON]) player->rotate(-30 * dt, Vector3(0, 1, 0));

		if (pad_state.axis[TRIGGERS_RT] > 0) {
			player->shoot();
			player->camera_info.z -= (0.01*cos(game->time * 25 * PI));
			player->camera_info.z -= 0.1 * dt;

		}
		else if (pad_state.axis[TRIGGERS_RT] == -1) player->camera_info.z = 0;
		

		if (pad_state.button[BACK_BUTTON]) exit(0);
		
		if (pad_state.button[RB_BUTTON]) player->accelerate();			

		//Change camera target
		if (pad_state.button[Y_BUTTON]) {
			//setTarget(game->test3);
		}
		if (pad_state.button[X_BUTTON]) {
			setTarget(game->player);
		}
		if (pad_state.button[B_BUTTON]) {
			if (game->time_scale == 1.0) game->time_scale = 0.01;
			else game->time_scale = 1.0;
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
		camera->lookAt(global_player_matrix * Vector3(0, 2, -25), global_player_matrix *  Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));
	}
}

Camera* Controller::getCamera()
{
	Fighter* fighter = (Fighter*)target;
	fighter->updateCamera(camera);
	return camera;
}

void Controller::setTarget(Entity* entity)
{
	target = entity;
	CollisionManager::instance->player_entity = entity;
}


//---------------------------------------------------------------------------------------------------------
ControllerIA::ControllerIA()
{
	dynamic_controller = false;
	attack = false;

	checkpoint = 0;
	s_checkpoints.push_back(Vector3(-185, 350, 385));
	s_checkpoints.push_back(Vector3(-324, 276, -167));
	s_checkpoints.push_back(Vector3(-415, 13, -433));
	s_checkpoints.push_back(Vector3(-20, 148, -485));
	s_checkpoints.push_back(Vector3(365, 250, -420));
	s_checkpoints.push_back(Vector3(403, 300, 111));
	s_checkpoints.push_back(Vector3(195, 340, 432));

}

ControllerIA::~ControllerIA()
{

}

void ControllerIA::update(float dt)
{
	Game* game = Game::instance;
	Fighter* player = (Fighter*)target;
	//PRUEBAS PARA IA 
	Camera* camera = game->current_camera;
	//orientarme donde esta el objetivo
	Matrix44 global_matrix = player->getGlobalMatrix();	
	Vector3 target_position = s_checkpoints[checkpoint];

	//std::cout << "distanceCam " << global_matrix.getTranslation().distance(camera->eye) << std::endl;
	if (global_matrix.getTranslation().distance(camera->eye) < 250){
		target_position = camera->eye;
		attack = true;
	}
	else attack = false;


	//std::cout << "distance " << global_matrix.getTranslation().distance(target_position) << std::endl;
	//std::cout << "s_checkpoint " << s_checkpoints.size() << " check " << checkpoint << std::endl;

	if (global_matrix.getTranslation().distance(target_position) < 100) {
		if (s_checkpoints.size()-1 == checkpoint) {
			checkpoint = 0;
		}
		else checkpoint++;
		return;
	}

	Vector3 front = global_matrix.rotateVector(Vector3(0, 0, -1));
	//vector direction que es desde la entidad hacia el objetivo 
	Vector3 to_target = global_matrix.getTranslation() - target_position;

	to_target.normalize();
	front.normalize();

	float angle = to_target.dot(front); //cos del angulo 
	Vector3 axis_ws = to_target.cross(front);
	Matrix44 global_inv = global_matrix;
	global_inv.inverse();
	Vector3 axis_ls = global_inv.rotateVector(axis_ws);

	//cuando los dos vectores sean iguales vaya de 1-0 0-1
	player->local_matrix.rotateLocal((1.0 - angle) * dt, axis_ls);

	//si tengo el avion inclinado 
	//cuanto tiene que rotar para que se alineara
	Vector3 top_ws = global_matrix.rotateVector(Vector3(0, 1, 0));
	angle = top_ws.dot(Vector3(0, 1, 0));
	player->local_matrix.rotateLocal((1.0 - angle) * dt, Vector3(0, 0, 1));

	//Vector3 prueba = player->local_matrix.frontVector();
	//debug_lines.push_back(prueba);
	//std::cout << "targe " << target_position.v << " player " << camera->eye.v << std::endl;
	if(attack) player->shoot();
	
}
