#ifndef STAGE_H
#define STAGE_H
#include "game.h"
#include "sound.h"
#include <iostream>
using namespace std;

class Stage {
public:
	const Uint8* keystate;
	JoystickState pad_state;
	Game* game;
	SoundManager* soundMng = SoundManager::getInstance();

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void update(double dt) = 0;

	virtual void onKeyPressed(SDL_KeyboardEvent event) = 0;
	virtual void onMouseButton(SDL_MouseButtonEvent event) = 0;
	virtual void onJoyButtonUp(SDL_JoyButtonEvent event) = 0;
	
	virtual ~Stage() {}
};

class StageIntro : public Stage {
public:

	void init();
	void render();
	void update(double dt);

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	~StageIntro() { cout << "intro: llamada al destructor." << endl; }
};

class StageMenu : public Stage {
public:
	int selected_plane;
	bool to_select_plane;
	bool lock;
	Camera* camera;
	Entity* plane_to_select;

	void init();
	void render();
	void update(double dt);
	void changePlane(int next);

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	~StageMenu() { cout << "menu: llamada al destructor." << endl; }
};

class StageLoading : public Stage {
public:
	void init();
	void render();
	void update(double dt);

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	~StageLoading() { cout << "Loading: llamada al destructor." << endl; }
};

class StagePlay : public Stage {
public:
	World* world;

	void init();
	void render();
	void update(double dt);

	void renderGUI();
	void stoppingGame();
	void printString(float x, float y, float z, void* font, float r, float g, float b, char * string);

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	~StagePlay();
};

class StageFinal : public Stage {
public:
	World* world;
	bool win;

	void init();
	void render();
	void update(double dt);

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	StageFinal(bool result) { win = result; }
	~StageFinal();
};

class StageDelegator : public Stage {
public:
	// constructor/destructor
	StageDelegator() : stage(new StageIntro()) { }
	virtual ~StageDelegator() { delete stage; }

private:
	// Interfaz de delegacion
	Stage* stage;

public:
	std::string type_stage = "Intro";
	void init() { stage->init(); }
	void render() { stage->render(); }
	void update(double dt) { stage->update(dt); }

	void onKeyPressed(SDL_KeyboardEvent event);
	void onMouseButton(SDL_MouseButtonEvent event);
	void onJoyButtonUp(SDL_JoyButtonEvent event);

	// atributos normales
	void toIntro() { delete stage; stage = new StageIntro(); type_stage = "Intro"; }
	void toMenu() { delete stage; stage = new StageMenu(); type_stage = "Menu"; }
	void toLoading() { delete stage; stage = new StageLoading(); type_stage = "Loading"; }
	void toPlay() { delete stage; stage = new StagePlay(); type_stage = "Play"; }
	void toFinal(bool result) { delete stage; stage = new StageFinal(result); type_stage = "Final"; }
};

#endif