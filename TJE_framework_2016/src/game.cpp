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

SDL_Joystick* pad = NULL;

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
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
    std::cout << " * Path: " << getPath() << std::endl;
	time_scale = 1.0;

	//Scene inicialization 
	world = World::getInstance();
	world->root = new Entity();

	ctrlPlayer = new Controller();
    //SDL_SetWindowSize(window, 50,50);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//Inicilizamos los diferentes managers
	TextureManager* textureMng = TextureManager::getInstance();
	MeshManager* meshMng = MeshManager::getInstance();
	bulletMng = BulletMaganer::getInstance();

	//create our camera
	free_camera = new Camera();
	free_camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	free_camera->setPerspective(70,window_width/(float)window_height,0.1,10000); //set the projection, we want to be perspective
	current_camera = free_camera;

	world->factory("data/worlds/world_test.txt");

	//Cargamos Meshes
	/*Shader* fog_shader = Shader::load("data/shaders/fog.vs", "data/.-------");
	fog_shader->enable();
	fog_shader->setVector3("u_fog_color", fog_color);*/

	//Avion para testear delete
	player = (Fighter*)world->createEntity(Vector3(0, 1000, 0));
	world->root->addChildren(player);

	player_camera = new Camera();
	player_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() *  Vector3(0, 0, 20), Vector3(0, 1, 0));
	current_camera = player_camera;
	
	//Llamar controller->pad si no hay pad devuelve un null
	pad = openJoystick(0);

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
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	current_camera->set();

	//Desactivar el depth buffer de openGL para pintar el skybox
	glDisable(GL_DEPTH_TEST);
	world->skybox->local_matrix.setTranslation(current_camera->eye.x, current_camera->eye.y, current_camera->eye.z);
	world->skybox->render(current_camera);
	glEnable(GL_DEPTH_TEST);
	

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
	bulletMng->render(current_camera);
    
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
	bulletMng->update(seconds_elapsed * time_scale);
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant


	//ctrlPlayer->update(seconds_elapsed);

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
	else if(current_camera == player_camera){


		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) player->rotate(-90 * seconds_elapsed, Vector3(1,0,0));
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) player->rotate(90 * seconds_elapsed, Vector3(1, 0, 0));
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) player->rotate(-90 * seconds_elapsed, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) player->rotate(90 * seconds_elapsed, Vector3(0, 1, 0));
		if (keystate[SDL_SCANCODE_Q] ) player->rotate(90 * seconds_elapsed * 0.5, Vector3(0, 0, -1));
		if (keystate[SDL_SCANCODE_E] ) player->rotate(90 * seconds_elapsed * 0.5, Vector3(0, 0, 1));
		

		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			
			if (abs(pad_state.axis[LEFT_ANALOG_Y]) > 0.1)
				player->rotate(90 * pad_state.axis[LEFT_ANALOG_Y] * seconds_elapsed, Vector3(1, 0, 0));
			if (abs(pad_state.axis[LEFT_ANALOG_X]) > 0.1)
				player->rotate(-90 * pad_state.axis[LEFT_ANALOG_X] * seconds_elapsed, Vector3(0, 0, 1));

			if (abs(pad_state.axis[RIGHT_ANALOG_X]) > 0.1)
				player->camera_info.x = pad_state.axis[RIGHT_ANALOG_X];
			else
				player->camera_info.x = 0;

			if (abs(pad_state.axis[RIGHT_ANALOG_Y]) > 0.1)
				player->camera_info.z = pad_state.axis[RIGHT_ANALOG_Y];
			else player->camera_info.z = 0;

			if (pad_state.button[RIGHT_ANALOG_BUTTON])
			{
				std::cout << "R1 " << std::endl;
				player->shoot();
			}

			/*VERSION CON TRIGGERS
			if (pad_state.axis[TRIGGERS] > -0.1)
			{				
				std::cout << "trigger " << pad_state.axis[TRIGGERS] << std::endl;
				player->camera_info.z = -1.0 * pad_state.axis[TRIGGERS];
			}else player->camera_info.z = 0;*/

			
		}


		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			player->rotate(mouse_delta.x * 0.03, Vector3(0, 0, 1));
			player->rotate(mouse_delta.y * -0.03, Vector3(1, 0, 0));
		}

		if (keystate[SDL_SCANCODE_F]) player->shoot();
	
		
	}

	Matrix44 global_player_matrix = player->getGlobalMatrix();
	//player_camera->lookAt(global_player_matrix * Vector3(0, 2, -5), global_player_matrix * Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));
	player->updateCamera(player_camera);


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
    

	//angle += seconds_elapsed * 10;
	/*
		ELIMINAR DE ENTITY TODAS LAS ENTIDADES DEL VECTOR to_destroy
	*/
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
			}
			else {
				time_scale = 1.0;
				current_camera = player_camera;
			}
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

