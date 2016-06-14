#ifndef STAGE_H
#define STAGE_H
#include "game.h"
#include <iostream>
using namespace std;

class Stage {
public:
	virtual void init() = 0;
	virtual void render() = 0;
	virtual bool update(double dt) = 0;
	virtual ~Stage() {}
};

class StageIntro : public Stage {
public:
	const Uint8* keystate;
	JoystickState pad_state;

	void init();
	void render();
	bool update(double dt);
	~StageIntro() { cout << "intro: llamada al destructor." << endl; }
};

class StageMenu : public Stage {
public:

	void init();
	void render();
	bool update(double dt);
	~StageMenu() { cout << "menu: llamada al destructor." << endl; }
};

class StagePlay : public Stage {
public:
	World* world;
	const Uint8* keystate;

	void init();
	void render();
	bool update(double dt);
	~StagePlay() { cout << "play: llamada al destructor." << endl; }
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
	void init() { stage->init(); }
	void render() { stage->render(); }
	bool update(double dt) { return stage->update(dt); }

	// atributos normales
	void toIntro() { delete stage; stage = new StageIntro(); }
	void toMenu() { delete stage; stage = new StageMenu(); }
	void toPlay() { delete stage; stage = new StagePlay(); }
};

#endif