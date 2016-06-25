#include "stage.h"
#include "camera.h"
#include "texture.h"

void StageIntro::init()
{
	//game = Game::instance;
}

void StageIntro::render()
{ 
	game = Game::instance;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Camera cam2D;

	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
	cam2D.set();

	Mesh quad;
	quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

	Texture* introImg = Texture::get("data/stage/PlanesTheVideoGame.tga");

	introImg->bind();
	quad.render(GL_TRIANGLES);
	introImg->unbind();

}

void StageIntro::update(double dt) 
{
	game = Game::instance;
	/*keystate = game->keystate;
	if (keystate[SDL_SCANCODE_SPACE]) return true;

	if (game->pad) {
		pad_state = getJoystickState(game->pad);
		if (pad_state.button[A_BUTTON] || pad_state.button[START_BUTTON]) return true;
	}
	
	return false; */
}

void StageIntro::onKeyPressed(SDL_KeyboardEvent event) {

}

void StageIntro::onMouseButton(SDL_MouseButtonEvent event) {

}

void StageIntro::onJoyButtonUp(SDL_JoyButtonEvent event) {

}

//---------------------------------------------------------------------------------------------------------
void StageMenu::init()
{
	//game = Game::instance;
	to_select_plane = false;
	lock = false;
	selected_plane = 0;

	game = Game::instance;
	camera = new Camera();
	camera->lookAt(Vector3(0, 10, 25), Vector3(0, 0, 0), Vector3(0, 1, 0)); //position the camera and point to 0,0,0
	camera->setPerspective(70, game->window_width / (float)game->window_height, 0.1, 10000); 

	plane_to_select = World::instance->entities[selected_plane];
}

void StageMenu::render() 
{
	game = Game::instance;

	if (!to_select_plane) {
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Camera cam2D;

		cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
		cam2D.set();

		Mesh quad;
		quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

		Texture* menu = Texture::get("data/stage/Menu2.tga");

		menu->bind();
		quad.render(GL_TRIANGLES);
		menu->unbind();
	}
	else {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->set();

		plane_to_select->render(camera);

		drawGrid(500); //background grid

		drawText(180,225, plane_to_select->name, Vector3(1, 1, 1), 2);

		glDisable(GL_BLEND);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Camera cam2D;
		cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
		cam2D.set();

		Mesh quad;
		quad.createQuad(game->window_width*0.5, game->window_height*0.5, game->window_width, game->window_height, true);

		Texture* menuSelection = Texture::get("data/stage/MenuSelection.tga");

		//ACtivar la transparencia
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		menuSelection->bind();
		quad.render(GL_TRIANGLES);
		menuSelection->unbind();
		glDisable(GL_BLEND);

	}
}

void StageMenu::update(double dt)
{
	game = Game::instance;
	if (to_select_plane) {
		//DEG2RAD
		plane_to_select->local_matrix.rotateLocal(dt * 0.5, Vector3(0, 1, 0));
		plane_to_select->update(dt);
	}
	/*keystate = game->keystate;
	if (game->pad) pad_state = getJoystickState(game->pad);

	if (!to_select_plane) {
	if (keystate[SDL_SCANCODE_SPACE]) to_select_plane = true;
	if (pad_state.button[B_BUTTON]) to_select_plane = true;
	}
	else {
	//DEG2RAD
	plane_to_select->local_matrix.rotateLocal(dt * 0.5, Vector3(0, 1, 0));
	plane_to_select->update(dt);


	/*if (pad_state.button[RIGHT_BUTTON]) changePlane( selected_plane+1 );
	if (pad_state.button[LEFT_BUTTON]) changePlane( selected_plane-1 );*/
	/*if (pad_state.button[RIGHT_BUTTON]) changePlane(0);
	if (pad_state.button[LEFT_BUTTON]) changePlane(1);
	if (pad_state.button[UP_BUTTON]) changePlane(2);
	if (pad_state.button[DOWN_BUTTON]) changePlane(3);

	if (pad_state.button[A_BUTTON])	return true;
	}

	return false;*/
}

void StageMenu::onKeyPressed(SDL_KeyboardEvent event) {
	switch (event.keysym.sym) {
	case SDLK_b:
		to_select_plane = true;
		break;
	case SDLK_RIGHT:
		changePlane(selected_plane + 1);
		break;
	case SDLK_LEFT:
		changePlane(selected_plane - 1);
		break;
	}
}

void StageMenu::onMouseButton(SDL_MouseButtonEvent event) {

}

void StageMenu::onJoyButtonUp(SDL_JoyButtonEvent event) {
	switch (event.button) {
	case RIGHT_BUTTON:
		changePlane(selected_plane + 1);
		break;
	case LEFT_BUTTON:
		changePlane(selected_plane - 1);
		break;
	case B_BUTTON:
		to_select_plane = true;
		break;
	}
}

void StageMenu::changePlane(int next)
{

	World* world = World::instance;

	if (next < 0) next = world->entities.size() - 1;
	else if (next >= world->entities.size()) next = 0;

	plane_to_select = world->entities[next];
	selected_plane = next;
	world->current_fighter = next;
}

//---------------------------------------------------------------------------------------------------------
void StageLoading::init()
{
	//game = Game::instance;
}

void StageLoading::render()
{
	game = Game::instance;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Camera cam2D;

	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
	cam2D.set();

	Mesh quad;
	quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

	Texture* loading = Texture::get("data/stage/Loading.tga");

	loading->bind();
	quad.render(GL_TRIANGLES);
	loading->unbind();
}

void StageLoading::update(double dt)
{
	game = Game::instance;
	/*keystate = game->keystate;
	if (keystate[SDL_SCANCODE_SPACE]) return true;

	if (game->pad) {
		pad_state = getJoystickState(game->pad);
		if (pad_state.button[B_BUTTON] || pad_state.button[START_BUTTON]) return true;
	}

	return false;*/

}

void StageLoading::onKeyPressed(SDL_KeyboardEvent event) {

}

void StageLoading::onMouseButton(SDL_MouseButtonEvent event) {

}

void StageLoading::onJoyButtonUp(SDL_JoyButtonEvent event) {
	
}

//---------------------------------------------------------------------------------------------------------
StagePlay::~StagePlay() {
	world = World::getInstance();
	world->root->children.clear();
	cout << "play: llamada al destructor." << endl;
}

void StagePlay::init()
{
	cout << "play: haciendo init()" << endl;
	game = Game::instance;
	world = World::getInstance();


	game->player = (Fighter*)world->createSpitfire(Vector3(0, 100, -200));
	game->player->dynamic_entity = true;
	game->player->onDemand();
	world->root->addChildren(game->player);

	game->player_camera = new Camera();
	game->player_camera->setPerspective(70, game->window_width / (float)game->window_height, 0.1, 10000);
	//game->player_camera->lookAt(game->player->getGlobalMatrix() * Vector3(0, 2, -5), game->player->getGlobalMatrix() *  Vector3(0, 0, 50), Vector3(0, 1, 0));


	game->ctrlPlayer = new Controller();
	game->ctrlPlayer->target = game->player;
	game->ctrlPlayer->pad = game->pad;

	game->ctrlPlayer->camera = game->player_camera;
	game->current_camera = game->ctrlPlayer->getCamera();
	/*
	for (int i = 0; i < 5; i++) {
		EntityCollider* test = new EntityCollider();
		test->setup("data/meshes/furniture/dartboard.obj", "data/textures/furniture/dartboard.tga");
		test->two_sided = true;

		Vector3 pos;
		pos.random(400);

		test->local_matrix.setTranslation(pos.x, pos.y, pos.z);
		test->dynamic_entity = true;
		test->onDemand();
		world->root->addChildren(test);
	}*/


	game->test3 = new Fighter(); 
	std::string name, texture;
	name = std::string("data/meshes/fighter/") + world->entities[world->current_fighter]->name + std::string("/") + world->entities[world->current_fighter]->name + std::string(".obj");
	texture = std::string("data/textures/fighters/") + world->entities[world->current_fighter]->name + std::string(".tga");

	game->test3 = (Fighter*)world->entities[world->current_fighter];
	game->test3->onDemand();
	game->test3->camera_center = Vector3(0, 9, 30);
	game->test3->camera_eye = Vector3(0, 8, -20);
	game->test3->setSpeed(100.0);
	game->test3->start = 10;
	game->test3->local_matrix.setTranslation(0, 100, -250);
	world->root->addChildren(game->test3);
}

void StagePlay::render()
{
	game = Game::instance;
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	game->current_camera->set();
	glEnable(GL_DEPTH_TEST);
	world->root->render(game->current_camera);
	world->skybox->render(game->current_camera);

	game->bulletMng->render(game->current_camera);

	game->renderDebug(game->current_camera);

	game->renderGUI();

	//std::string posCamera = std::string("Camera pos: ") + std::to_string(int(game->current_camera->eye.x)) + std::string(" ") + std::to_string(int(game->current_camera->eye.y)) + std::string(" ") + std::to_string(int(game->current_camera->eye.z));
	//drawText(2, 2, posCamera, Vector3(1, 1, 1), 2);
	glDisable(GL_DEPTH_TEST);
}

void StagePlay::update(double dt)
{
	game = Game::instance;
	world->root->update(dt);

	game->ctrlPlayer->update(dt);
	game->collisionMng->update(dt);

	game->bulletMng->update(dt);

	//Nuevos controladores ubicados en world y no en game
	for (int i = 0; i < world->controllers.size(); i++) {
		if (world->controllers[i]->target->destroy_entity)continue;
		world->controllers[i]->update(dt);
	}

/*	for (int i = 0; i < game->controllers.size(); i++) {
		if (game->controllers[i]->target->destroy_entity)continue;
		game->controllers[i]->update(dt);
	}*/


	/*keystate = game->keystate;
	if (keystate[SDL_SCANCODE_K]) return true;
	return false;*/
}

void StagePlay::onKeyPressed(SDL_KeyboardEvent event) {
	switch (event.keysym.sym)
	{
	case SDLK_TAB:
		if (game->time_scale == 1.0) {
			game->time_scale = 0.01;
			game->free_camera->lookAt(game->current_camera->eye, game->current_camera->center, game->current_camera->up);
			game->current_camera = game->free_camera;
			game->ctrlPlayer->active = false;
		}
		else {
			game->time_scale = 1.0;
			game->current_camera = game->ctrlPlayer->camera;
			game->ctrlPlayer->active = true;
		}
		break;
	case SDLK_1:
		game->ctrlPlayer->target = game->test3;
		break;
	case SDLK_2:
		game->ctrlPlayer->target = game->player;
		break;
	}
}

void StagePlay::onMouseButton(SDL_MouseButtonEvent event) {

}

void StagePlay::onJoyButtonUp(SDL_JoyButtonEvent event) {
	switch (event.button) {
	case 0:
		std::cout << "JoyEvent -> 0" << std::endl;
		break;
	case 1:
		std::cout << "JoyEvent -> 1" << std::endl;
		break;
	case 2:
		std::cout << "JoyEvent -> 2" << std::endl;
		break;
	case 3:
		std::cout << "JoyEvent -> 3" << std::endl;
		break;
	case 4:
		std::cout << "JoyEvent -> 4" << std::endl;
		break;
	case 5:
		std::cout << "JoyEvent -> 5" << std::endl;
		break;
	case 6:
		std::cout << "JoyEvent -> 6" << std::endl;
		break;
	case 7:
		std::cout << "JoyEvent -> 7" << std::endl;
		break;
	case 8:
		std::cout << "JoyEvent -> 8" << std::endl;
		break;
	case 9:
		std::cout << "JoyEvent -> 9" << std::endl;
		break;
	case 10:
		std::cout << "JoyEvent -> 10" << std::endl;
		break;
	case 11:
		std::cout << "JoyEvent -> 11" << std::endl;
		break;
	case 12:
		std::cout << "JoyEvent -> 12" << std::endl;
		break;
	case 13:
		std::cout << "JoyEvent -> 13" << std::endl;
		break;
	default:
		std::cout << "JoyEvent -> Otro" << std::endl;
		break;
	}
}

//---------------------------------------------------------------------------------------------------------
void StageDelegator::onKeyPressed(SDL_KeyboardEvent event) {
	if (type_stage == "Intro") {
		toMenu();
		stage->init();
	}
	else if (type_stage == "Menu") {
		StageMenu * sm = (StageMenu*)stage;
		if (event.keysym.sym == SDLK_a && sm->to_select_plane) {
			toLoading();
			stage->init();
		}
		else {
			stage->onKeyPressed(event);
		}
	}
	else if (type_stage == "Loading" && (event.keysym.sym == SDLK_SPACE || event.keysym.sym == SDLK_RETURN)) {
		toPlay();
		stage->init();
	}
	else if (type_stage == "Play") {
		stage->onKeyPressed(event);
	}
}

void StageDelegator::onMouseButton(SDL_MouseButtonEvent event) {
	if (type_stage == "Intro") {
		toMenu();
		stage->init();
	}
	else if (type_stage == "Menu") {
		//toLoading();
		//stage->init();
	}
	else if (type_stage == "Loading") {
		//toPlay();
		//stage->init();
	}
	else if (type_stage == "Play") {
		stage->onMouseButton(event);
	}
}

void StageDelegator::onJoyButtonUp(SDL_JoyButtonEvent event) {
	if (type_stage == "Intro") {
		toMenu();
		stage->init();
	}
	else if (type_stage == "Menu") {
		StageMenu * sm = (StageMenu*)stage;
		if (event.button == A_BUTTON && sm->to_select_plane) {
			toLoading();
			stage->init();
		}
		else {
			stage->onJoyButtonUp(event);
		}
	}
	else if (type_stage == "Loading" && event.button == B_BUTTON) {
		toPlay();
		stage->init();
	}
	else if (type_stage == "Play") {
		stage->onJoyButtonUp(event);
	}
}