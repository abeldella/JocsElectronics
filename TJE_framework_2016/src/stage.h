#ifndef STAGE_H
#define STAGE_H
#include "game.h"
#include <iostream>
using namespace std;

class Stage {
public:
	const Uint8* keystate;
	JoystickState pad_state;
	Game* game;

	virtual void init() = 0;
	virtual void render() = 0;
	virtual bool update(double dt) = 0;
	virtual ~Stage() {}
};

class StageIntro : public Stage {
public:

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

class StageLoading : public Stage {
public:
	void init();
	void render();
	bool update(double dt);
	~StageLoading() { cout << "Loading: llamada al destructor." << endl; }
};

class StagePlay : public Stage {
public:
	World* world;

	void init();
	void render();
	bool update(double dt);
	~StagePlay();
};

class StageDelegator : public Stage {
public:
	// constructor/destructor
	StageDelegator() : stage(new StagePlay()) { }
	virtual ~StageDelegator() { delete stage; }

private:
	// Interfaz de delegacion
	Stage* stage;

public:
	std::string type_stage = "Intro";
	void init() { stage->init(); }
	void render() { stage->render(); }
	bool update(double dt) { return stage->update(dt); }

	// atributos normales
	void toIntro() { delete stage; stage = new StageIntro(); type_stage = "Intro"; }
	void toMenu() { delete stage; stage = new StageMenu(); type_stage = "Menu"; }
	void toLoading() { delete stage; stage = new StageLoading(); type_stage = "Loading"; }
	void toPlay() { delete stage; stage = new StagePlay(); type_stage = "Play"; }
};

#endif