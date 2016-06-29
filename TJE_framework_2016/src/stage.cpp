#include "stage.h"
#include "camera.h"
#include "texture.h"

#include "bass.h"

#include "includes.h"

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
	if (game->time > 90) return;
	
	quad.createQuad(game->window_width * 0.5, 1200 + 20 * -game->time, 800, 2400, true);
	//quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

	Texture* introImg = Texture::get("data/stage/Intro.tga");

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

void StageIntro::onKeyPressed(SDL_KeyboardEvent event) 
{

}

void StageIntro::onMouseButton(SDL_MouseButtonEvent event) 
{

}

void StageIntro::onJoyButtonUp(SDL_JoyButtonEvent event) 
{

}

//---------------------------------------------------------------------------------------------------------
void StageMenu::init()
{
	game = Game::instance;
	BASS_ChannelStop(game->hSampleChannel);
	//sound test
	HSAMPLE hSample = 0;
	HCHANNEL hSampleChannel = 0;

	BASS_Init(1, 44100, 0, 0, NULL);
	const char* filename = "sounds/Shoot_To_Thrill.wav";
	hSample = BASS_SampleLoad(false, filename, 0, 0, 3, 0); 

	if (hSample == 0) {
		int err = BASS_ErrorGetCode();
		std::cerr << "Error [" << err << "] while loading sample " << filename << std::endl;
	}

	if (hSampleChannel == 0) {
		hSampleChannel = BASS_SampleGetChannel(hSample, false);
	}
	if (hSampleChannel == 0) {
		int err = BASS_ErrorGetCode();
		if (err != BASS_ERROR_NOCHAN)
			std::cerr << "Error [" << err << "] no channel id" << std::endl;
		return;
	}
	BOOL result = BASS_ChannelPlay(hSampleChannel, true);
	if (result == FALSE)
		std::cerr << "Error [" << BASS_ErrorGetCode() << "] while playing sample" << std::endl;

	//Fin sound test

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

	
		if (pad_state.button[RIGHT_BUTTON]) changePlane(0);
		if (pad_state.button[LEFT_BUTTON]) changePlane(1);
		if (pad_state.button[UP_BUTTON]) changePlane(2);
		if (pad_state.button[DOWN_BUTTON]) changePlane(3);

		if (pad_state.button[A_BUTTON])	return true;
	}

	return false;*/
}

void StageMenu::onKeyPressed(SDL_KeyboardEvent event) 
{
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

void StageMenu::onMouseButton(SDL_MouseButtonEvent event) 
{

}

void StageMenu::onJoyButtonUp(SDL_JoyButtonEvent event) 
{
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

void StageLoading::onKeyPressed(SDL_KeyboardEvent event)
{

}

void StageLoading::onMouseButton(SDL_MouseButtonEvent event)
{

}

void StageLoading::onJoyButtonUp(SDL_JoyButtonEvent event)
{

}

//---------------------------------------------------------------------------------------------------------
StagePlay::~StagePlay() {
	/*world = World::getInstance();
	for (int i = 0; i < world->root->children.size(); i++) {
		if (world->root->children[i]->name == "")continue;

		world->root->children[i]->destroy_entity = true;
	}
	world->hordes = 0;*/
	//world->root->children.clear();
	cout << "play: llamada al destructor." << endl;
}

void StagePlay::init()
{
	cout << "play: haciendo init()" << endl;
	game = Game::instance;
	world = World::getInstance();

	/*game->player = (Fighter*)world->createSpitfire(Vector3(0, 100, -200));
	game->player->dynamic_entity = true;
	game->player->onDemand();
	world->root->addChildren(game->player);*/

	game->player = new Fighter();
	std::string name, texture;
	name = std::string("data/meshes/fighter/") + world->entities[world->current_fighter]->name + std::string("/") + world->entities[world->current_fighter]->name + std::string(".obj");
	texture = std::string("data/textures/fighters/") + world->entities[world->current_fighter]->name + std::string(".tga");

	game->player = (Fighter*)world->entities[world->current_fighter];
	game->player->onDemand();
	game->player->camera_center = Vector3(0, 9, 30);
	game->player->camera_eye = Vector3(0, 8, -20);
	game->player->setSpeed(100.0);
	game->player->start = 10;
	game->player->local_matrix.setTranslation(0, 100, -250);
	world->root->addChildren(game->player);

	game->player_camera = new Camera();
	game->player_camera->setPerspective(70, game->window_width / (float)game->window_height, 0.1, 10000);

	game->ctrlPlayer = new Controller();
	game->ctrlPlayer->target = game->player;
	game->ctrlPlayer->pad = game->pad;

	game->ctrlPlayer->camera = game->player_camera;
	game->current_camera = game->ctrlPlayer->getCamera();
	
	world->createHorde("data/worlds/world_hordes.txt");
	
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

	renderGUI();
	if (game->time_scale == 0) stoppingGame();

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


	if (world->enemies <= 0 && world->hordes < MAX_HORDES) {
		game->time_scale = 0;
		world->createHorde("data/worlds/world_hordes.txt");
	}

	StageDelegator* sd = game->current_stage;
	if ( game->player->life <= 0) {
		sd->toFinal(false);
		sd->init();
	}

}

void StagePlay::onKeyPressed(SDL_KeyboardEvent event) 
{
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
		//game->ctrlPlayer->target = game->test3;
		break;
	case SDLK_2:
		game->ctrlPlayer->target = game->player;
		break;
	}
}

void StagePlay::onMouseButton(SDL_MouseButtonEvent event) 
{

}

void StagePlay::onJoyButtonUp(SDL_JoyButtonEvent event) 
{
	switch (event.button) {
	case 0:
		//std::cout << "JoyEvent -> 0" << std::endl;
		break;
	case 1:
		//std::cout << "JoyEvent -> 1" << std::endl;
		break;
	case 2:
		//std::cout << "JoyEvent -> 2" << std::endl;
		break;
	case 3:
		//std::cout << "JoyEvent -> 3" << std::endl;
		break;
	case 4:
		//std::cout << "JoyEvent -> 4" << std::endl;
		break;
	case 5:
		//std::cout << "JoyEvent -> 5" << std::endl;
		break;
	case 6:
		//std::cout << "JoyEvent -> 6" << std::endl;
		break;
	case 7:
		//std::cout << "JoyEvent -> 7" << std::endl;
		break;
	case 8:
		//std::cout << "JoyEvent -> 8" << std::endl;
		break;
	case 9:
		//std::cout << "JoyEvent -> 9" << std::endl;
		break;
	case 10:
		//std::cout << "JoyEvent -> 10" << std::endl;
		break;
	case 11:
		//std::cout << "JoyEvent -> 11" << std::endl;
		break;
	case 12:
		//std::cout << "JoyEvent -> 12" << std::endl;
		break;
	case 13:
		//std::cout << "JoyEvent -> 13" << std::endl;
		break;
	default:
		//std::cout << "JoyEvent -> Otro" << std::endl;
		break;
	}
}

void StagePlay::printString(float x, float y, float z, void* font, float r, float g, float b, char * string) {
	/*glColor3f(r, g, b);
	glRasterPos3f(x, y, z);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);*/
}

void StagePlay::renderGUI()
{
	int wWidth = game->window_width;
	int wHeight = game->window_height;

	std::string current_life = std::to_string(((Fighter*)game->ctrlPlayer->target)->life);
	std::string boss_life = std::to_string(world->boss->life);
	std::string remaining_enemies = std::to_string(world->enemies);
	std::string time = std::to_string(game->time);
	std::string horde = std::to_string(world->hordes);

	drawText(wWidth*0.055, wHeight*0.025, current_life, Vector3(0.4, 1, 0.4), 3);
	drawText(wWidth*0.425, wHeight*0.025, boss_life, Vector3(0.4, 1, 0.4), 2);
	drawText(wWidth*0.425, wHeight*0.085, time, Vector3(0.4, 1, 0.4), 2);
	drawText(wWidth*0.425, wHeight*0.117, horde, Vector3(0.4, 1, 0.4), 2);

	if (world->enemies > 0) {
		drawText(wWidth*0.425, wHeight*0.055, remaining_enemies, Vector3(0.4, 1, 0.4), 2);
	}
	else {
		drawText(200, 60, std::string("YOU WIN!!"), Vector3(0.4, 1, 0.4), 3);
		StageDelegator* sd = game->current_stage;
		sd->toFinal(true);
		sd->init();
	}

	//Clear screen colors
	glColor3f(1,1,1);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	/* Funcion para printar pos de enemigos
	Vector3 pos = world->boss->getGlobalMatrix() * world->boss->mesh->center;
	
	std::string info = "[  ]";
	char *cstr = new char[ info.length() ];
	strcpy(cstr, info.c_str());
	printString(pos.x, pos.y, pos.z, GLUT_BITMAP_TIMES_ROMAN_10, 0, 1, 0, cstr);
	delete[] cstr;*/
	

	Camera cam2D;
	cam2D.setOrthographic(0, wWidth, wHeight, 0, -1, 1);
	cam2D.set();

	Mesh quad, hub, map;
	quad.createQuad(wWidth*0.5, wHeight*0.5, wWidth*0.4, wHeight*0.4, true);
	hub.createQuad(wWidth*0.5, wHeight*0.5, wWidth, wHeight, true);

	map.createQuad(wWidth*0.1, wHeight*0.9, wWidth*0.2, wHeight*0.2, true);
	Texture* Tmap = Texture::get("data/textures/woodeuro2.TGA");
	
	//Aviones del mapa
	Mesh points, playerPoint;
	Vector3 posPlayer = game->player->getGlobalMatrix().getTranslation();
	posPlayer = (posPlayer / world->floor->mesh->halfSize) * (wWidth*0.1);
	posPlayer.y = posPlayer.z;
	posPlayer.z = 0;
	posPlayer.x += wWidth*0.1;
	posPlayer.y += wHeight*0.9;
	playerPoint.vertices.push_back(posPlayer);

	for (int i = 0; i < Fighter::s_fighters.size(); i++) {
		Fighter* fighter = Fighter::s_fighters[i];
		Vector3 pos = fighter->getGlobalMatrix().getTranslation();
		
		pos = (pos / world->floor->mesh->halfSize) * (wWidth*0.1);
		pos.y = pos.z;
		pos.z = 0;

		pos.x += wWidth*0.1;
		pos.y += wHeight*0.9;
		points.vertices.push_back(pos);
	}


	Texture* crosshair = Texture::get("data/textures/HUD/crosshair.tga");
	Texture* Thub = Texture::get("data/textures/HUD/HUB.tga");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	crosshair->bind();
	quad.render(GL_TRIANGLES);
	crosshair->unbind();
	Thub->bind();
	hub.render(GL_TRIANGLES);
	Thub->unbind();
	//MAP
	Tmap->bind();
	map.render(GL_TRIANGLES);
	Tmap->unbind();

	glPointSize(3);
	glColor4f(1, 0, 0, 1);
	points.render(GL_POINTS);
	glColor4f(0, 1, 0, 1);
	playerPoint.render(GL_POINTS);
	glPointSize(1);
	glDisable(GL_BLEND);

	//Clear screen colors
	glColor3f(1, 1, 1);

}

void StagePlay::stoppingGame()
{
	game = Game::instance;
	int wWidth = game->window_width;
	int wHeight = game->window_height;

	drawText(wWidth*0.1, wHeight*0.15, std::string("Presiona A para continuar"), Vector3(1, 1, 1), 3);

	if (game->pad) {
		pad_state = getJoystickState(game->pad);
		if (pad_state.button[A_BUTTON])  game->time_scale = 1.0;
	}
}

//---------------------------------------------------------------------------------------------------------
void StageFinal::init()
{
	world = World::getInstance();
	for (int i = 0; i < world->root->children.size(); i++) {
		if (world->root->children[i]->name == "")continue;

		world->root->children[i]->destroy_entity = true;
	}
	world->hordes = 0;
}

void StageFinal::render()
{
	game = Game::instance;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Camera cam2D;

	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
	cam2D.set();

	Mesh quad;
	quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

	Texture* loading;
	if(win) loading = Texture::get("data/stage/FinalWin.tga");
	else loading = Texture::get("data/stage/FinalLose.tga");

	loading->bind();
	quad.render(GL_TRIANGLES);
	loading->unbind();
}

void StageFinal::update(double dt)
{

}

void StageFinal::onKeyPressed(SDL_KeyboardEvent event)
{

}

void StageFinal::onMouseButton(SDL_MouseButtonEvent event)
{

}

void StageFinal::onJoyButtonUp(SDL_JoyButtonEvent event)
{

}
//---------------------------------------------------------------------------------------------------------
void StageDelegator::onKeyPressed(SDL_KeyboardEvent event) 
{
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

void StageDelegator::onMouseButton(SDL_MouseButtonEvent event) 
{
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

void StageDelegator::onJoyButtonUp(SDL_JoyButtonEvent event) 
{
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
	else if (type_stage == "Loading" && event.button == A_BUTTON) {
		toPlay();
		stage->init();
		game = Game::instance;
	}
	else if (type_stage == "Play") {
		stage->onJoyButtonUp(event);
	}
	else if (type_stage == "Final"  && event.button == A_BUTTON) {
		toMenu();
		init();
	}
}