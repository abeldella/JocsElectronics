#include <string>
#include <algorithm>
#include "world.h"
#include "includes.h"
#include "entity.h"
#include "extra/textparser.h"
#include "mesh.h"
#include "texture.h"

#define FIGHTER 1
#define FIGHTERWITHPOS 2
#define TERRAIN 3
#define SKYBOX 4
#define BOSS 5

World* World::instance = NULL;

World::World()
{
	root = NULL;
	skybox = NULL;
	
}


Entity* World::factory(const char* filename)
{

	TextParser t;
	std::string docTexture;

	//Cargamos un archivo ASE
	if (!t.create(filename)) {
		std::cout << "File: world.cpp - TextParser::create() fail" << std::endl;
		exit(0);
	}

	t.seek("*SCENE_NUMOBJECTS");
	int total_doc_entities = t.getint();

	for (int k = 0; k < total_doc_entities; k++) {

		t.seek("*NUMOBJECT");
		int num_entities = t.getint();
		int id = t.getint();
		std::string type = t.getword();

		//std::transform(type.begin(), type.end(), type.begin(), ::tolower);

		switch(id)
		{
		case FIGHTER:
			{
				for (int i = 0; i < num_entities; i++) {
					createPlane();
				}
			}
			break;

		case FIGHTERWITHPOS:
			{
				for (int i = 0; i < num_entities; i++) {
					Vector3 pos = Vector3(t.getfloat(), t.getfloat(), t.getfloat());
					docCreateEntity(pos);
				}
			}
			break;

		case TERRAIN:
			createTerrain();
			break;
		case SKYBOX:
			createSkybox();
			break;
		case BOSS:
			docTexture = t.getword();
			createBoss(type.c_str() , docTexture.c_str());
			break;
		
		default: std::cout << "Incorrect Id: " << id << std::endl;
			break;
		
		}
		/*Debe leer nombre de la entidad, mesh, lod_mesh, texture y shader
		hacemos entity->mesh = Mesh::get(" nombre buscado en archivo");
		return entitu
		*/
	}
	return NULL;
}

void World::createSkybox()
{
	skybox = new EntityMesh();
	skybox->setup("data/meshes/skybox/cubemap.ASE", "data/textures/cielo.TGA");
	skybox->local_matrix.setScale(100, 100, 100);
	skybox->frustum_test = false;
}

void World::createPlane()
{
	for (int i = 0; i < 100; i++) {

		EntityMesh* entity = new EntityMesh();
		entity->setup("data/meshes/spitfire/spitfire.ASE", "data/textures/spitfire_color_spec.TGA", "data/meshes/spitfire/spitfire_low.ASE");
		Vector3 pos;
		pos.random(1000);
		entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);
		root->addChildren(entity);
	}

}

void World::docCreateEntity(Vector3 pos)
{
	EntityMesh* entity = new EntityMesh();
	entity->setup("data/meshes/spitfire/spitfire.ASE", "data/textures/spitfire_color_spec.TGA", "data/meshes/spitfire/spitfire_low.ASE");
	entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	root->addChildren(entity);

}

Entity* World::createEntity(Vector3 pos)
{
	EntityMesh* entity = new Fighter();
	entity->setup("data/meshes/spitfire/spitfire.ASE", "data/textures/spitfire_color_spec.TGA", "data/meshes/spitfire/spitfire_low.ASE");
	entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	
	return entity;
}

void World::createTerrain()
{
	for (int i = -3; i <= 3; i++) {
		for (int j = -3; j <= 3; j++) {
			EntityMesh* island = new EntityMesh();
			island->setup("data/meshes/island.ASE", "data/textures/island_color_luz.TGA");
			island->local_matrix.setTranslation( i*island->mesh->halfSize.x * 2, 0, j*island->mesh->halfSize.z);
			//island->shader = fog_shader;
			root->addChildren(island);
		}
	}

	EntityMesh* sea = new EntityMesh();
	sea->setup("data/meshes/agua.ASE", "data/textures/agua.TGA");
	sea->local_matrix.setTranslation(0, 0, 0);
	//root->addChildren(sea);
}

void World::createBoss(const char* name, const char* texture)
{
	std::string n_filename, t_filename;
	n_filename = std::string("data/meshes/boss/") + name;
	t_filename = std::string("data/textures/") + texture;

	Fighter* boss = new Fighter();
	boss->setup(n_filename.c_str(), t_filename.c_str());
	Vector3 pos;
	pos.random(1000);
	boss->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	root->addChildren(boss);
}
