#include <string>
#include <algorithm>
#include "world.h"
#include "includes.h"
#include "entity.h"
#include "extra/textparser.h"
#include "mesh.h"
#include "texture.h"

#define FIGHTER 1
#define ENTITY 2
#define TERRAIN 3
#define SKYBOX 4
#define BOSS 5

World* World::instance = NULL;

World::World()
{
	root = NULL;
	skybox = NULL;
	boss = NULL;

	current_fighter = 0;
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

		switch (id)
		{
		case FIGHTER:
		{
			docTexture = t.getword();
			for (int i = 0; i < num_entities; i++) {
				Vector3 pos = Vector3(t.getfloat(), t.getfloat(), t.getfloat());
				createFighter(type.c_str(), docTexture.c_str(), pos);
			}
		}
			break;

		case ENTITY:
		{
			docTexture = t.getword();
			for (int i = 0; i < num_entities; i++) {
				Vector3 pos = Vector3(t.getfloat(), t.getfloat(), t.getfloat());
				int angle = t.getint();
				Vector3 rotation = Vector3(t.getfloat(), t.getfloat(), t.getfloat());

				docCreateEntity(type.c_str(), docTexture.c_str(), pos, angle, rotation);
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
		{
		docTexture = t.getword();
		Vector3 pos = Vector3(t.getfloat(), t.getfloat(), t.getfloat());
		createBoss(type.c_str(), docTexture.c_str(), pos);
		}
			break;
		
		default: std::cout << "Incorrect Id: " << id << std::endl;
			break;
		
		}
										
	}


	t.seek("*MENU_PLANESELECTION");
	int total_planes = t.getint();

	for (int i = 0; i < total_planes; i++) {
		std::string type = t.getword();
		menuPlaneSelection(type.c_str());
	}

	return NULL;
}

void World::createSkybox()
{
	skybox = new EntityCollider();
	skybox->setup("data/meshes/skybox/cubemap.OBJ", "data/textures/room2.tga");
	skybox->frustum_test = false;
	skybox->onDemand();
	skybox->local_matrix.setTranslation(0, 200, 0);

}

void World::createFighter(const char* name, const char* texture, Vector3 pos)
{
	std::string n_filename, t_filename;
	n_filename = std::string("data/meshes/") + name;
	t_filename = std::string("data/textures/") + texture;


	Fighter* fighter = new Fighter();
	fighter->setup(n_filename.c_str(), t_filename.c_str());
	fighter->local_matrix.setTranslation(pos.x, pos.y, pos.z);

	fighter->name = "fighter";
	fighter->dynamic_entity = true;
	fighter->setTimetoShoot(0.7);
	fighter->setSpeed(100.0);
	fighter->onDemand();

	ControllerIA* ctrlFighter = new ControllerIA();
	ctrlFighter->dynamic_controller = true;
	ctrlFighter->target = fighter;

	root->addChildren(fighter);
	controllers.push_back(ctrlFighter);
}

void World::docCreateEntity(const char* name, const char* texture, Vector3 pos, int angle, Vector3 rotation)
{
	std::string n_filename, t_filename;
	n_filename = std::string("data/meshes/") + name;
	t_filename = std::string("data/textures/") + texture;


	EntityCollider* entity = new EntityCollider();
	entity->setup(n_filename.c_str(), t_filename.c_str());
	entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);

	entity->local_matrix.rotateLocal(angle * DEG2RAD, rotation);

	entity->onDemand();
	root->addChildren(entity);

}

Entity* World::createSpitfire(Vector3 pos)
{
	EntityMesh* entity = new Fighter();
	entity->setup("data/meshes/spitfire/spitfire.ASE", "data/textures/spitfire_color_spec.TGA", "data/meshes/spitfire/spitfire_low.ASE");
	entity->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	
	return entity;
}

void World::createTerrain()
{

//Creamos el suelo de la habitación
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 1; j++) {
			Mesh* plane = new Mesh();
			//plane->createPlane(700);
			plane->createPlane(600);
			EntityMesh* floor = new EntityMesh();
			floor->mesh = plane;
			//floor->texture = Texture::get("data/TilesPlain0136_1_S.TGA");
			floor->texture = Texture::get("data/textures/woodeuro.TGA");
			floor->two_sided = true;
			floor->local_matrix.setTranslation(i*floor->mesh->halfSize.x * 2, -10, j*floor->mesh->halfSize.z * 2);
			root->addChildren(floor);
		}
	}

}

void World::createBoss(const char* name, const char* texture, Vector3 pos)
{
	std::string n_filename, t_filename;
	n_filename = std::string("data/meshes/") + name;
	t_filename = std::string("data/textures/") + texture;


	Fighter* boss = new Fighter();
	boss->setup(n_filename.c_str(), t_filename.c_str());
	boss->local_matrix.setTranslation(pos.x, pos.y, pos.z);
	
	boss->dynamic_entity = true;
	boss->setTimetoShoot(1.0);
	boss->setSpeed(130.0);
	boss->onDemand();

	ControllerIA* ctrlBoss = new ControllerIA();
	ctrlBoss->dynamic_controller = true;
	ctrlBoss->target = boss;

	this->boss = boss;
	root->addChildren(boss);
	controllers.push_back(ctrlBoss);
	
}

void World::menuPlaneSelection(const char* name)
{
	std::string n_filename, t_filename;
	n_filename = std::string("data/meshes/fighter/") + name + std::string("/") + name + std::string(".obj");
	t_filename = std::string("data/textures/fighters/") + name + std::string(".tga");

	Fighter* fighter = new Fighter();
	fighter->setup(n_filename.c_str(), t_filename.c_str());
	fighter->name = name;

	entities.push_back(fighter);
}