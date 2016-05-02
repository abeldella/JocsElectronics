#ifndef WORLD_H
#define WORLD_H

#include "framework.h"
#include <vector>

class Entity;
class EntityMesh;

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

	Entity* root = NULL;
	EntityMesh* skybox = NULL;
	std::vector< Entity* > entities;

	//Metodos para cmabiar la textura de skybox ej. Cambia a noche.

	//Metodo Factory o create entity
	Entity* factory(const char* filename);

};


#endif