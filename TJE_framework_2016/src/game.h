/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "bullets.h"
#include "world.h"
#include "controller.h"
#include "manager.h"
#include "sound.h"
#include "bass.h"

class StageDelegator;
class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

    float time;
	float start_play_time;
	float time_scale; //game mov speed

	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Camera* current_camera; //active
	Camera* free_camera; //our global camera
	Camera* player_camera; //player

	Fighter* player;
	Controller* ctrlPlayer;

	SDL_Joystick* pad;

	Game(SDL_Window* window);
	void init( void );
	void render( void );
	void update( double dt );

	void onKeyPressed( SDL_KeyboardEvent event );
	void onMouseButton( SDL_MouseButtonEvent event );
	void onJoyButtonUp( SDL_JoyButtonEvent event );
    void onResize( SDL_Event e );
    
	void setWindowSize(int width, int height);
	void renderDebug(Camera* camera);

	World* world;
	BulletMaganer* bulletMng;
	CollisionManager* collisionMng;
	SoundManager* soundMng;

	StageDelegator* current_stage;

};


#endif 