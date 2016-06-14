#ifndef WORLD_H
#define WORLD_H

#include "framework.h"
#include <vector>
#include "entity.h"
#include "controller.h"

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

	Entity* root;
	EntityCollider* skybox;
	std::vector< Entity* > entities;

	std::vector< Controller* > controllers;

	//Entidad boss para testear delete
	Fighter* boss;

	//Metodos para cmabiar la textura de skybox ej. Cambia a noche.

	//Metodo Factory o create entity
	Entity* factory(const char* filename);

	void createSkybox();
	
	Entity* createEntity(Vector3 pos);

	void createTerrain();

	void createFighter(const char* name, const char* texture, Vector3 pos);
	void createBoss(const char* name, const char* texture, Vector3 pos);
	void docCreateEntity(const char* name, const char* texture, Vector3 pos, int angle, Vector3 rotation);

};


#endif