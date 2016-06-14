#include "stage.h"
#include "camera.h"
#include "texture.h"

void StageIntro::init()
{
	cout << "intro: haciendo inti()" << endl;
}
void StageIntro::render()
{ 
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Game* game = Game::instance;
	Camera cam2D;

	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);
	cam2D.set();

	Mesh quad;
	quad.createQuad(game->window_width * 0.5, game->window_height * 0.5, game->window_width, game->window_height, true);

	Texture* introImg = Texture::get("data/stage/PlanesTheVideoGame.tga");

	//ACtivar la transparencia
	introImg->bind();
	quad.render(GL_TRIANGLES);
	introImg->unbind();

}

bool StageIntro::update(double dt) 
{ 
	Game* game = Game::instance;
	keystate = game->keystate;
	if (keystate[SDL_SCANCODE_SPACE]) return true;

	if (game->pad) {
		pad_state = getJoystickState(game->pad);
		if (pad_state.button[A_BUTTON] || pad_state.button[START_BUTTON]) return true;
	}
	cout << "intro: haciendo update()" << endl;
	
	return false; 


}

//---------------------------------------------------------------------------------------------------------
void StageMenu::init()
{
	cout << "menu: haciendo init()" << endl;

}
void StageMenu::render() 
{ cout << "menu: haciendo redner()" << endl; 
}

bool StageMenu::update(double dt) 
{ cout << "menu: haciendo update()" << endl; return false; 


}


//---------------------------------------------------------------------------------------------------------
void StagePlay::init()
{
	cout << "play: haciendo init()" << endl;
	Game* game = Game::instance;
	world = World::getInstance();
	//world->root = new Entity();

	world->factory("data/worlds/world_test.txt");

	game->player = (Fighter*)world->createEntity(Vector3(0, 100, -200));
	game->player->dynamic_entity = true;
	game->player->onDemand();
	world->root->addChildren(game->player);

	game->player_camera = new Camera();
	game->player_camera->setPerspective(70, game->window_width / (float)game->window_height, 0.1, 10000);
	game->player_camera->lookAt(game->player->getGlobalMatrix() * Vector3(0, 2, -5), game->player->getGlobalMatrix() *  Vector3(0, 0, 50), Vector3(0, 1, 0));


	game->ctrlPlayer = new Controller();
	game->ctrlPlayer->target = game->player;
	game->ctrlPlayer->pad = game->pad;

	game->ctrlPlayer->camera = game->player_camera;
	game->current_camera = game->ctrlPlayer->getCamera();


	EntityMesh* bed = new EntityMesh();
	bed->setup("data/meshes/furniture/bed.obj", "data/textures/furniture/bed.tga");
	bed->local_matrix.setTranslation(0, -10, 600 - (bed->mesh->center.y + 50));
	world->root->addChildren(bed);

	for (int i = 0; i < 5; i++) {
		EntityCollider* test = new EntityCollider();
		test->setup("data/meshes/furniture/dartboard.obj", "data/textures/furniture/dartboard.tga");
		test->two_sided = true;

		Vector3 pos;
		pos.random(600);

		test->local_matrix.setTranslation(pos.x, pos.y, pos.z);
		test->dynamic_entity = true;
		test->onDemand();
		world->root->addChildren(test);
	}

	EntityCollider* test2 = new EntityCollider();
	test2->setup("data/meshes/furniture/door.obj", "data/textures/furniture/door.tga");
	test2->two_sided = true;
	test2->local_matrix.setTranslation(605, -10, -600 - (test2->mesh->center.x * 4));
	test2->local_matrix.rotateLocal(90 * DEG2RAD, Vector3(0, 1, 0));
	test2->onDemand();
	world->root->addChildren(test2);

	game->test3 = new Fighter();
	game->test3->setup("data/meshes/fighter/CFA44/CFA44.obj", "data/meshes/fighter/CFA44/CFA44.tga");
	game->test3->two_sided = true;
	game->test3->onDemand();
	game->test3->camera_center = Vector3(0, 9, 30);
	game->test3->camera_eye = Vector3(0, 8, -20);
	game->test3->local_matrix.setTranslation(0, 100, -250);
	world->root->addChildren(game->test3);


	for (int f = 0; f < 5; f++) {
		Fighter* enemy = new Fighter();
		enemy->setup("data/meshes/fighter/X-29A/X-29A.OBJ", "data/meshes/fighter/X-29A/X-29A.tga");
		enemy->setTimetoShoot(0.7);
		enemy->onDemand();

		Vector3 pos;
		pos.random(400);
		enemy->local_matrix.setTranslation(pos.x, pos.y, pos.z);

		ControllerIA* ctrlEnemy = new ControllerIA();
		ctrlEnemy->dynamic_controller = true;
		ctrlEnemy->target = enemy;


		world->root->addChildren(enemy);
		game->controllers.push_back(ctrlEnemy);
	}



}
void StagePlay::render()
{
	Game* game = Game::instance;

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

	std::string posCamera = std::string("Camera pos: ") + std::to_string(int(game->current_camera->eye.x)) + std::string(" ") + std::to_string(int(game->current_camera->eye.y)) + std::string(" ") + std::to_string(int(game->current_camera->eye.z));
	drawText(2, 2, posCamera, Vector3(1, 1, 1), 2);
	glDisable(GL_DEPTH_TEST);
}

bool StagePlay::update(double dt)
{
	Game* game = Game::instance;
	world->root->update(dt);

	game->ctrlPlayer->update(dt);
	game->collisionMng->update(dt);

	game->bulletMng->update(dt);

	//Nuevos controladores ubicados en world y no en game
	for (int i = 0; i < world->controllers.size(); i++) {
		if (world->controllers[i]->target->destroy_entity)continue;
		world->controllers[i]->update(dt);
	}

	for (int i = 0; i < game->controllers.size(); i++) {
		if (game->controllers[i]->target->destroy_entity)continue;
		game->controllers[i]->update(dt);
	}


	keystate = game->keystate;
	if (keystate[SDL_SCANCODE_SPACE]) return true;
	return false;
}