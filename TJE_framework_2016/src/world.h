#ifndef WORLD_H
#define WORLD_H

#include "framework.h"
#include <vector>
#include "entity.h"

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
	EntityMesh* skybox;
	std::vector< Entity* > entities;

	//Entidad boss para testear delete
	Fighter* boss;

	//Metodos para cmabiar la textura de skybox ej. Cambia a noche.

	//Metodo Factory o create entity
	Entity* factory(const char* filename);

	void createSkybox();
	void createFighter();
	Entity* createEntity(Vector3 pos);
	void docCreateEntity(Vector3 pos);
	void createTerrain();
	void createBoss(const char* name, const char* texture);

};


#endif