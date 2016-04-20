#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"

#include <cmath>

//some globals
Mesh* mesh = NULL;
Mesh* mesh_low = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;

Game* Game::instance = NULL;

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
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
    std::cout << " * Path: " << getPath() << std::endl;
    
    //SDL_SetWindowSize(window, 50,50);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	camera->setPerspective(70,window_width/(float)window_height,0.1,10000); //set the projection, we want to be perspective

	//create a plane mesh
	mesh = new Mesh();
	mesh_low = new Mesh();
	//mesh->createPlane(10);
	//Cargamos una malla en la variable mesh
	long t1 = getTime();
	if (mesh->loadASE("data/meshes/spitfire/spitfire.ASE") == false) {
		std::cout << "Mesh can not be loaded" << std::endl;
		exit(0);
	}
	
	if (mesh_low->loadASE("data/meshes/spitfire/spitfire_low.ASE") == false) {
		std::cout << "Mesh can not be loaded" << std::endl;
		exit(0);
	}
	//Cargamos la mesh a la VRAM
	mesh->uploadToVRAM();
	mesh_low->uploadToVRAM();
	long t2 = getTime();
	std::cout << "Mesh load time : " << ((t2-t1)*0.001) << "s" << std::endl;

	//Cargamos los shaders
	shader = new Shader();
	if( !shader->load("data/shaders/simple.vs","data/shaders/simple.fs") )
	{
		std::cout << "shader not found or error" << std::endl;
		exit(0);
	}

	//Creamos e inicilizamos una textura
	texture = new Texture();
	if (texture->load("data/textures/spitfire_color_spec.TGA") == false) {
		std::cout << "Texture can not be loaded" << std::endl;
		exit(0);
	}

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
	camera->set();

	//Draw out world
	drawGrid(500); //background grid
    
	//create model matrix from plane
	Matrix44 m;
    m.setScale(1,1,1);
	//m.rotate(angle * DEG2RAD, Vector3(0,1,0) ); //build a rotation matrix

	//draw the plane
	if(0) //render using shader
	{
	    Matrix44 mvp = m * camera->viewprojection_matrix;

		shader->enable();
		shader->setMatrix44("u_model", m );
		shader->setMatrix44("u_mvp", mvp );
   
		mesh->render(GL_TRIANGLES, shader);
		shader->disable();
	}
	else //render using fixed pipeline (DEPRECATED)
	{
		/*glPushMatrix();
		m.multGL();
		texture->bind();
		mesh->render(GL_TRIANGLES);
		texture->unbind();
		glPopMatrix();*/
		float render_halfSize;
		for (int j = -50; j < 50; j++) {
			for (int i = -50; i < 50; i++) {
				Vector3 pos(i * 10, j * 10, 0);
				m.setTranslation(pos.x, pos.y, pos.z);
				Mesh * render_mesh = mesh;
				if (pos.distance(camera->eye) > 30) {
					render_mesh = mesh_low;
				}
				render_halfSize = render_mesh->halfSize.length();
				glPushMatrix();
				m.multGL();
				texture->bind();
				if (camera->clipper.SphereInFrustum(pos.x + render_mesh->center.x, pos.y + render_mesh->center.y, pos.z + render_mesh->center.z, render_halfSize)) {
					render_mesh->render(GL_TRIANGLES);
				}
				texture->unbind();
				glPopMatrix();
			}
		}
	}
	
    
    glDisable( GL_BLEND );

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//mouse input to rotate the cam
	if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(mouse_delta.x * 0.005, Vector3(0,-1,0));
		camera->rotate(mouse_delta.y * 0.005, camera->getLocalVector( Vector3(-1,0,0)));
	}

	//async input to move the camera around
	if(keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
	if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0,0,1) * speed);
	if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0,0,-1) * speed);
	if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1,0,0) * speed);
	if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1,0,0) * speed);
    
	//to navigate with the mouse fixed in the middle
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
    

	angle += seconds_elapsed * 10;
}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); //ESC key, kill the app
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
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

