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
	boss = NULL;	
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
					//createFighter();
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
	skybox->setup("data/meshes/skybox/cubemap.ASE", "data/prueba3.tga");
	//skybox->setup("data/meshes/skybox/cubemap.ASE", "data/textures/cielo.TGA");
	//skybox->local_matrix.setTranslation(0, skybox->mesh->halfSize.y, 0);
	//skybox->local_matrix.setScale(40, 50, 40);
	skybox->local_matrix.setScale(20, 25, 20);
	skybox->frustum_test = false;

	/*for (int i = -4; i < 5; i++) {
		for (int j = -4; j < 5; j++) {
			Mesh* plane = new Mesh();
			plane->createPlane(1000);
			EntityMesh* floor = new EntityMesh();
			floor->mesh = plane;
			floor->texture = Texture::get("data/textura_luna.tga");
			floor->two_sided = true;
			floor->local_matrix.setTranslation(i*floor->mesh->halfSize.x * 2, 7000, j*floor->mesh->halfSize.z * 2);
			root->addChildren(floor);
		}
	}
	Mesh* plane = new Mesh();
	plane->createPlane(100000);
	EntityMesh* floor = new EntityMesh();
	floor->mesh = plane;
	floor->texture = Texture::get("data/textura_luna.tga");
	floor->two_sided = true;
	floor->local_matrix.setTranslation(floor->mesh->halfSize.x * 2, 7000, floor->mesh->halfSize.z * 2);
	root->addChildren(floor);*/
}

void World::createFighter()
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
	//entity->setup("data/meshes/boss/Arc170_2.obj", "data/textures/ARC170.tga");
	entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	
	return entity;
}

void World::createTerrain()
{
	/*for (int i = -3; i <= 3; i++) {
		for (int j = -3; j <= 3; j++) {
			EntityMesh* island = new EntityMesh();
			
			
			island->setup("data/meshes/island.ASE", "data/textures/island_color_luz.TGA");
			island->local_matrix.setTranslation( i*island->mesh->halfSize.x * 2, 0, j*island->mesh->halfSize.z);
			//island->shader = fog_shader;
			root->addChildren(island);
		}
	}*/

//Creamos el suelo de la habitación
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 1; j++) {
			Mesh* plane = new Mesh();
			//plane->createPlane(700);
			plane->createPlane(1000);
			EntityMesh* floor = new EntityMesh();
			floor->mesh = plane;
			//floor->texture = Texture::get("data/TilesPlain0136_1_S.TGA");
			floor->texture = Texture::get("data/woodeuro.TGA");
			floor->two_sided = true;
			floor->local_matrix.setTranslation(i*floor->mesh->halfSize.x * 2, -10, j*floor->mesh->halfSize.z * 2);
			root->addChildren(floor);
		}
	}/*
	Mesh* plane = new Mesh();
	plane->createPlane(100000);
	EntityMesh* floor = new EntityMesh();
	floor->mesh = plane;
	floor->texture = Texture::get("data/TilesPlain0136_1_S.TGA");
	floor->two_sided = true;
	floor->local_matrix.setTranslation(0,-10,0);
	root->addChildren(floor);
	*/
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

	Vector3 pos;
	pos.random(1000);

	Fighter* boss = new Fighter();
	boss->setup(n_filename.c_str(), t_filename.c_str());
	boss->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	
	boss->dynamic_entity = true;
	boss->onDemand();

	root->addChildren(boss);
	this->boss = boss;
	
}
