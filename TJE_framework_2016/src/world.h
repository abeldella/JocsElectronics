#ifndef WORLD_H
#define WORLD_H

#include "framework.h"
#include <vector>
#include "entity.h"
#include "controller.h"

#define MAX_HORDES 2
#define FLOOR_SIZE 600 

class World
{
private:
	World();
	
public:
	static World* instance;
	static World* getInstance() {
		if (instance == NULL)
			instance = new World();
		return instance;
	}

	int current_fighter; //Fighter control on menu selection
	int hordes;			// round or Id horde
	int enemies;		//current horede

	Entity* root;
	EntityCollider* skybox;
	EntityCollider* floor;
	Fighter* boss;
	std::vector< Entity* > entities;		//Fighters to be selected
	std::vector< Controller* > controllers;	//Enemy controllers

	//Metodos para cmabiar la textura de skybox ej. Cambia a noche.

	//Metodo Factory o create entity
	Entity* factory(const char* filename);
	Entity* createSpitfire(Vector3 pos);
	
	void createSkybox();
	void createTerrain();

	void createFighter(const char* name, const char* texture, Vector3 pos, float velocity, int life); //create enemy
	void createBoss(const char* name, const char* texture, Vector3 pos, float velocity, int life);
	void docCreateEntity(const char* name, const char* texture, Vector3 pos, int angle, Vector3 rotation);
	void menuPlaneSelection(const char* name);

	void createHorde(const char* filename);

};


#endif