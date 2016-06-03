/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This contains several functions that can be useful when programming your game.
*/
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

#include "includes.h"
#include "framework.h"

//General functions **************
long getTime();

//generic purposes fuctions
void drawGrid(float dist);
bool drawText(float x, float y, std::string text, Vector3 c, float scale = 1);

//check opengl errors
bool checkGLErrors();

std::string getPath();

Vector2 getDesktopSize( int display_index = 0 );


std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);


//mapped as in SDL
enum XBOXpad
{
	//axis
	LEFT_ANALOG_X = 0,
	LEFT_ANALOG_Y = 1,
	RIGHT_ANALOG_X = 2,
	RIGHT_ANALOG_Y = 3,
	TRIGGERS_LT = 4,
	TRIGGERS_RT = 5,//both triggers share an axis (positive is right, negative is left trigger)

	//buttons
	UP_BUTTON = 0,
	DOWN_BUTTON = 1,
	LEFT_BUTTON = 2,
	RIGHT_BUTTON = 3,
	START_BUTTON = 4,
	BACK_BUTTON = 5,
	LB_BUTTON = 6,
	RB_BUTTON = 7,
	LEFT_ANALOG_BUTTON = 8,
	RIGHT_ANALOG_BUTTON = 9,
	A_BUTTON = 10,
	B_BUTTON = 11,
	X_BUTTON = 12,
	Y_BUTTON = 13
};

enum HATState
{
	HAT_CENTERED = 0x00,
	HAT_UP = 0x01,
	HAT_RIGHT = 0x02,
	HAT_DOWN = 0x04,
	HAT_LEFT = 0x08,
	HAT_RIGHTUP = (HAT_RIGHT | HAT_UP),
	HAT_RIGHTDOWN = (HAT_RIGHT | HAT_DOWN),
	HAT_LEFTUP = (HAT_LEFT | HAT_UP),
	HAT_LEFTDOWN = (HAT_LEFT | HAT_DOWN)
};

struct JoystickState
{
	int num_axis;	//num analog sticks
	int num_buttons; //num buttons
	float axis[8]; //analog stick
	char button[16]; //buttons
	HATState hat; //digital pad
};

SDL_Joystick* openJoystick(int num_joystick); //Create joystick
JoystickState getJoystickState(SDL_Joystick* joystick); //update jockstick state




#endif
