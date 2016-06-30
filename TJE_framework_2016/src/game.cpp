#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"
#include "entity.h"
#include "stage.h"
#include <cmath>

RenderToTexture* rt = NULL;
Game* Game::instance = NULL;

std::vector< Vector3 > debug_lines;


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
	collisionMng = NULL;
	soundMng = NULL;
	pad = NULL;

	current_stage = NULL;
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
	soundMng = SoundManager::getInstance();

	//create our camera
	free_camera = new Camera();
	free_camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	free_camera->setPerspective(70,window_width/(float)window_height,0.1,25000); //set the projection, we want to be perspective
	current_camera = free_camera;


	world->factory("data/worlds/world_test.txt");



	//Sounds
	//Streams
	soundMng->loadStream("StarWarsIntro.wav");
	soundMng->loadStream("air-force-radio.wav");
	soundMng->loadStream("Shoot_To_Thrill.wav");
	soundMng->loadStream("win.wav");
	soundMng->loadStream("Aurora_Tom_Clancy.wav");
	//samples
	soundMng->loadSample("triple-shot.wav");
	soundMng->loadSample("lose.wav");
	soundMng->loadSample("Pa_Panamericano.wav");
	//soundMng->loadSample("win.wav");

	//Cargamos Meshes
	/*Shader* fog_shader = Shader::load("data/shaders/fog.vs", "data/.-------");
	fog_shader->enable();
	fog_shader->setVector3("u_fog_color", fog_color);*/

	//Probando los Stages
	current_stage = new StageDelegator();
	current_stage->init();

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
/*	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	current_camera->set();
*/
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
	/*world->root->render(current_camera);
	world->skybox->render(current_camera);

	bulletMng->render(current_camera);

	renderDebug(current_camera);
	
	renderGUI();*/

				//STAGES
	current_stage->render();

	/*float time_per_frame = (getTime() - last);
	float fps = 1000.0 / time_per_frame;
	if (frame % 4 == 0)
		this->fps = fps;
	drawText(2, 2, std::string("FPS: ") + std::to_string(int(this->fps)), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	drawText(2, 20, std::string("DIPs: ") + std::to_string(Mesh::s_num_meshes_rendered), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	last = getTime();*/

	
	//Pintamos esto para saber la posición del avion
	/*std::string posCamera = std::string("Camera pos: ") + std::to_string(int(current_camera->eye.x)) + std::string(" ") + std::to_string(int(current_camera->eye.y)) + std::string(" ") + std::to_string(int(current_camera->eye.z));
	drawText(2, 2, posCamera, Vector3(1, 1, 1), 2);
	*/

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}


void Game::update(double seconds_elapsed)
{

			//STAGES
	/*if (current_stage->update(seconds_elapsed* time_scale) )
	{
		std::cout << "Cambio de stage" << std::endl;
		if (current_stage->type_stage == "Intro") current_stage->toMenu();
		else if (current_stage->type_stage == "Menu") current_stage->toLoading();
		else if (current_stage->type_stage == "Loading") current_stage->toPlay();
		else if (current_stage->type_stage == "Play") current_stage->toMenu();

		current_stage->init();
	}*/

	current_stage->update(seconds_elapsed* time_scale);


	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

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
		glColor3f(0, 0, 0);
		mesh.render(GL_POINTS);
		//mng->pointsOfCollision.vertices.resize(0);
	}
	
	glColor3f(1, 1, 1);
	glEnable(GL_DEPTH_TEST);

}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	if (event.keysym.sym == SDLK_ESCAPE) {
		exit(0);
	}
	else {
		current_stage->onKeyPressed(event);
	}
}

//Joystick event handler (sync input)
void Game::onJoyButtonUp(SDL_JoyButtonEvent event) {
	current_stage->onJoyButtonUp(event);
	
}

//Mouse event handler (sync input)
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

