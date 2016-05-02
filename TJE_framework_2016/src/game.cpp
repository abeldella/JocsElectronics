#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"
#include "entity.h"
#include "world.h"
#include <cmath>

//some globals
Entity* root = NULL;
Mesh* mesh = NULL;
Mesh* mesh_low = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;
Game* Game::instance = NULL;

World* world = NULL;

EntityMesh* island = NULL;
EntityMesh* sea = NULL;
EntityMesh* test = NULL;

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
    
	//Scene inicialization 
	world = World::getInstance();
	root = new Entity();

    //SDL_SetWindowSize(window, 50,50);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//Inicilizamos los diferentes managers
	TextureManager* textureMng = TextureManager::getInstance();
	MeshManager* meshMng = MeshManager::getInstance();
	//ShaderManager* shaderMng = ShaderManager::getInstance();

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	camera->setPerspective(70,window_width/(float)window_height,0.1,10000); //set the projection, we want to be perspective
		

	//Skybox 
	world->skybox = new EntityMesh();
	world->skybox->setup("data/meshes/skybox/cubemap.ASE", "data/textures/cielo.TGA");
	world->skybox->local_matrix.setScale(100, 100, 100);
	world->skybox->frustum_test = false;

	//Cargamos Meshes
	//long t1 = getTime();
	mesh = Mesh::get("data/meshes/spitfire/spitfire.ASE");
	mesh_low = Mesh::get("data/meshes/spitfire/spitfire_low.ASE");
	//long t2 = getTime();
	//std::cout << "Mesh load time : " << ((t2 - t1)*0.001) << "s" << std::endl;
	//Cargamos texturas
	texture = Texture::get("data/textures/spitfire_color_spec.TGA");
	
	//EntityMesh* prev_entity = NULL;
	for (int i = 0; i < 100; i++) {

		EntityMesh* entity = new EntityMesh();
		entity->mesh = mesh;
		entity->lod_mesh = mesh_low;
		entity->texture = texture;
		Vector3 pos;
		pos.random(1000);
		entity->local_matrix.setTranslation( pos.x, pos.y, pos.z );
		
		root->addChildren(entity);

	}
	//Avion para testear delete
	test = new EntityMesh();
	test->mesh = mesh;
	test->lod_mesh = mesh_low;
	test->texture = texture;
	Vector3 pos = camera->eye;
	test->local_matrix.setTranslation( pos.x, pos.y-10, pos.z-10 );
	//test->local_matrix.rotateLocal(3.14, Vector3(0, -1, 0));
	//test->local_matrix.rotateLocal(0.6, Vector3(-1, 0, 0));
	root->addChildren(test);

	for (int i = -3; i <= 3; i++) {
		for (int j = -3; j <= 3; j++) {
			island = new EntityMesh();
			island->setup("data/meshes/island.ASE","data/textures/island_color_luz.TGA");
			island->local_matrix.setTranslation(i*island->mesh->halfSize.x, -1000, j*island->mesh->halfSize.z);
			root->addChildren(island);
		}
	}

	sea = new EntityMesh();
	sea->setup("data/meshes/agua.ASE", "data/textures/agua.TGA");
	sea->local_matrix.setTranslation(0,-1020,0);
	//root->addChildren(sea);


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
	camera->set();

	//Desactivar el depth buffer de openGL para pintar el skybox
	glDisable(GL_DEPTH_TEST);
	world->skybox->local_matrix.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	world->skybox->render( camera );
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
	drawGrid(500); //background gridx
	root->render( camera );

    
	/*//create model matrix from plane
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
		glPushMatrix();
		m.multGL();
		texture->bind();
		mesh->render(GL_TRIANGLES);
		texture->unbind();
		glPopMatrix();
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
				if (camera->testSphereInFrustum(pos + mesh->center, render_halfSize)) {
					render_mesh->render(GL_TRIANGLES);
				}
				texture->unbind();
				glPopMatrix();
			}
		}
	}*/

    
    glDisable( GL_BLEND );

	/*float time_per_frame = (getTime() - last);
	float fps = 1000.0 / time_per_frame;
	if (frame % 4 == 0)
		this->fps = fps;
	drawText(2, 2, std::string("FPS: ") + std::to_string(int(this->fps)), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	drawText(2, 20, std::string("DIPs: ") + std::to_string(Mesh::s_num_meshes_rendered), this->fps > 30 ? Vector3(1, 1, 1) : Vector3(1, 0, 0), 2);
	last = getTime();
	Mesh::s_num_meshes_rendered = 0;*/


	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//root->update(seconds_elapsed);

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

