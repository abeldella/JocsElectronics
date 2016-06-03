#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"
#include "entity.h"
#include <cmath>

//some globals
//float angle = 0;
RenderToTexture* rt = NULL;
Game* Game::instance = NULL;

std::vector< Vector3 > debug_lines;

AntiAircraft* torreta;
ControllerIA* ctrlIA;

Game::Game(SDL_Window* window)
{
	this->window = window;
	instance = this;

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT
	// things create here cannot access opengl
	SDL_GetWindowSize( window, &window_width, &window_height );
	std::cout << " * Window size: " << window_width << " x " << window_height << std::endl;


	keystate = NULL;
	mouse_locked = false;
	world = NULL;
	bulletMng = NULL;
	pad = NULL;
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
    std::cout << " * Path: " << getPath() << std::endl;
	time_scale = 1.0;

	//Scene inicialization
	world = World::getInstance();
	world->root = new Entity();


	pad = openJoystick(0);
    //SDL_SetWindowSize(window, 50,50);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//Inicilizamos los diferentes managers
	TextureManager* textureMng = TextureManager::getInstance();
	MeshManager* meshMng = MeshManager::getInstance();
	bulletMng = BulletMaganer::getInstance();
	collisionMng = CollisionManager::getInstance();

	//create our camera
	free_camera = new Camera();
	free_camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	free_camera->setPerspective(70,window_width/(float)window_height,0.1,25000); //set the projection, we want to be perspective
	current_camera = free_camera;

	world->factory("data/worlds/world_test.txt");

	//Cargamos Meshes
	/*Shader* fog_shader = Shader::load("data/shaders/fog.vs", "data/.-------");
	fog_shader->enable();
	fog_shader->setVector3("u_fog_color", fog_color);*/

	//Avion para testear delete
	player = (Fighter*)world->createEntity(Vector3(0,100,-1000));
	player->dynamic_entity = true;
	player->onDemand();
	world->root->addChildren(player);

	player_camera = new Camera();
	player_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() *  Vector3(0, 0, 50), Vector3(0, 1, 0));
	


	ctrlPlayer = new Controller();
	ctrlPlayer->target = player;
	ctrlPlayer->pad = pad;

	ctrlPlayer->camera = player_camera;
	current_camera = ctrlPlayer->getCamera();

	EntityMesh* bed = new EntityMesh();
	bed->setup("data/bed.obj", "data/bed.tga");
	bed->local_matrix.setTranslation(0, -10, 1000 - (bed->mesh->center.y + 50));
	world->root->addChildren(bed);

	for (int i = 0; i < 5; i++) {
		EntityCollider* test = new EntityCollider();
		test->setup("data/test/dartboard.obj", "data/test/dartboard.tga");
		test->two_sided = true;
		
		Vector3 pos;
		pos.random(1000);

		test->local_matrix.setTranslation(pos.x, pos.y, pos.z);
		test->dynamic_entity = true;
		test->onDemand();
		world->root->addChildren(test);
	}

	EntityCollider* test2 = new EntityCollider();
	test2->setup("data/test/ok1/door.obj", "data/test/ok1/door.tga");
	test2->two_sided = true;
	test2->local_matrix.setTranslation(1005, -10, -1000 - (test2->mesh->center.x * 4));
	test2->local_matrix.rotateLocal(90 * DEG2RAD, Vector3(0, 1, 0));
	test2->onDemand();
	world->root->addChildren(test2);


	test3 = new Fighter();
	test3->setup("data/test/CFA44.obj", "data/test/CFA44.tga");
	test3->two_sided = true;
	test3->onDemand();
	test3->camera_center = Vector3(0, 9, 30);
	test3->camera_eye = Vector3(0, 8, -20);
	test3->local_matrix.setTranslation(0,100,-1500);
	world->root->addChildren(test3);

	//PRUEBA DE IA
	bosstest = world->boss;


	torreta = new AntiAircraft();
	torreta->setup("data/meshes/torreta/sci_fi_turret.obj", "data/meshes/torreta/sci_fi_turret.tga");
	torreta->onDemand();
	torreta->name = "sci_fi_turret";
	torreta->local_matrix.setTranslation(0, -10, -200);
	world->root->addChildren(torreta);

	ctrlIA = new ControllerIA();
	ctrlIA->target = torreta;


	/*	Codigo para composición de meshes, por ejemplo avion con misil.
		if (prev_entity) {
		prev_entity->addChildren(entity);
	}
	else
		root->addChildren(entity);
	prev_entity = entity;*/

	//Cargamos los shaders
	/*shader = new Shader();
	if( !shader->load("data/shaders/simple.vs","data/shaders/simple.fs") )
	{
		std::cout << "shader not found or error" << std::endl;
		exit(0);
	}*/

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	current_camera->set();

	//Desactivar el depth buffer de openGL para pintar el skybox
	/*glDisable(GL_DEPTH_TEST);
	world->skybox->local_matrix.setTranslation(current_camera->eye.x, current_camera->eye.y, current_camera->eye.z);
	world->skybox->render(current_camera);
	glEnable(GL_DEPTH_TEST);*/


	/*Hack de la camara para el ejemplo del planeta lejano creo que serviria para el ejemplo de la cabina
	Camera camera2;
	camera2 = *camera;
	camera2.lookAt( Vector3(0,0,0), camera->center - camera->eye, Vector3(0,1,0) );
	camera2.set();		//Cambiar las camaras en opengl
	planet->render( &camera2 );
	camera->set();		//Volvemos a la camara inicial

	Lo mismo con el skybox_root (Entity) - Podemos poner un rotation en update()
	skybox_root->local_matrix.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	skybox_root->render(camera);

	glClear( GL_DEPTH_BUFFER_BIT );
	*/

	//Draw out world
	world->root->render(current_camera);
	world->skybox->render(current_camera);

	bulletMng->render(current_camera);

	renderDebug(current_camera);

    glDisable( GL_BLEND );

	/*float time_per_frame = (getTime() - last);
	float fps = 1000.0 / time_per_frame;
	if (frame % 4 == 0)
		this->fps = fps;
	drawText(2, 2, std::string("FPS: ") + std::to_string(int(this->fps)), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	drawText(2, 20, std::string("DIPs: ") + std::to_string(Mesh::s_num_meshes_rendered), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	last = getTime();*/

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	world->root->update(seconds_elapsed * time_scale);
	ctrlPlayer->update(seconds_elapsed * time_scale);
	collisionMng->update(seconds_elapsed * time_scale);

	bulletMng->update(seconds_elapsed * time_scale);

	//controller IA Torreta
	ctrlIA->update(seconds_elapsed);

	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant


	if (!world->boss->destroy_entity) {

		//PRUEBAS PARA IA 
		Camera* camera = current_camera;
		//donde esta la camara
		Vector3 target_position = camera->eye;

		//orientarme donde esta la camara
		Matrix44 global_matrix = bosstest->getGlobalMatrix();

		Vector3 front = global_matrix.rotateVector(Vector3(0, 0, -1));
		//vector direction que es desde el objetvivo(boss) hacia la camara 
		Vector3 to_target = global_matrix.getTranslation() - target_position;

		to_target.normalize();
		front.normalize();

		float angle = to_target.dot(front); //cos del angulo 
		Vector3 axis_ws = to_target.cross(front);
		Matrix44 global_inv = global_matrix;
		global_inv.inverse();
		Vector3 axis_ls = global_inv.rotateVector(axis_ws);

		float dt = seconds_elapsed;
		//cuando los dos vectores sean iguales vaya de 1-0 01
		bosstest->local_matrix.rotateLocal((1.0 - angle) * dt, axis_ls);

		//si tengo el avion inclinado 
		//cuanto tiene que rotar para que se alineara
		Vector3 top_ws = global_matrix.rotateVector(Vector3(0, 1, 0));
		angle = top_ws.dot(Vector3(0, 1, 0));
		bosstest->local_matrix.rotateLocal((1.0 - angle) * dt, Vector3(0, 0, 1));

		Vector3 prueba = bosstest->local_matrix.frontVector();
		//debug_lines.push_back(prueba);
	}



	//FIN PRUEBAS IA



	if (current_camera == free_camera) {

		//mouse input to rotate the cam
		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			current_camera->rotate(mouse_delta.x * 0.005, Vector3(0, -1, 0));
			current_camera->rotate(mouse_delta.y * 0.005, current_camera->getLocalVector(Vector3(-1, 0, 0)));
		}

		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_LCTRL]) speed *= 0.1; //move slower with left ctrl
		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) current_camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) current_camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) current_camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) current_camera->move(Vector3(-1, 0, 0) * speed);
	}



	if (mouse_locked)
	{
		int center_x = floor(window_width*0.5);
		int center_y = floor(window_height*0.5);
        //center_x = center_y = 50;
		SDL_WarpMouseInWindow(this->window, center_x, center_y); //put the mouse back in the middle of the screen
		//SDL_WarpMouseGlobal(center_x, center_y); //put the mouse back in the middle of the screen

        this->mouse_position.x = center_x;
        this->mouse_position.y = center_y;
	}

	/*
		ELIMINAR DE ENTITY TODAS LAS ENTIDADES DEL VECTOR to_destroy
	*/
}

void Game::renderDebug(Camera* camera)
{
	CollisionManager* mng = CollisionManager::getInstance();

	//glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (mng->pointsOfCollision.vertices.size() > 1) {
		Mesh mesh;
		mesh.vertices = mng->pointsOfCollision.vertices;
		glColor3f(0, 1, 0);
		mesh.render(GL_POINTS);
		//mng->pointsOfCollision.vertices.resize(0);
	}
	
	glColor3f(1, 1, 1);
	glEnable(GL_DEPTH_TEST);

}
//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); //ESC key, kill the app
		case SDLK_TAB:
			if (time_scale == 1.0) {
				time_scale = 0.01;
				free_camera->lookAt(current_camera->eye, current_camera->center, current_camera->up);
				current_camera = free_camera;
				ctrlPlayer->active = false;
			}
			else {
				time_scale = 1.0;
				current_camera = ctrlPlayer->camera;
				ctrlPlayer->active = true;
			}
			break;
		case SDLK_1:
			std::cout << "target test 3" << std::endl;
			ctrlPlayer->target = test3;
			break;
		case SDLK_2:
			std::cout << "target player" << std::endl;
			ctrlPlayer->target = player;
			break;
	}
}


void Game::onMouseButton( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::setWindowSize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;

	/*
    Uint32 flags = SDL_GetWindowFlags(window);
    if(flags & SDL_WINDOW_ALLOW_HIGHDPI)
    {
        width *= 2;
        height *= 2;
    }
	*/

	glViewport( 0,0, width, height );
	free_camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

