
#include "manager.h"
#include "mesh.h"
#include "entity.h"
#include "camera.h"
#include "game.h"


CollisionManager* CollisionManager::instance = NULL;

CollisionManager::CollisionManager()
{
	assert(instance == NULL); //must be only one
	instance = this;
}

void CollisionManager::update(float dt)
{
	if (bulletToStatic()) std::cout << "BulletToStatic Colisiona!!!" << std::endl;
	if (bulletToDynamic()) std::cout << "BulletToDynamic Colisiona!!!" << std::endl;

	
}

bool CollisionManager::bulletToStatic()
{
	bManager = BulletMaganer::getInstance();

	for (int j = 0; j < static_entities.size(); j++) {

		EntityCollider* sEntity = static_entities[j];
		sEntity->mesh->collision_model->setTransform(sEntity->getGlobalMatrix().m);

		for (int i = 0; i < MAX_BULLETS; i++) {	
			
			if (bManager->bullets[i].ttl <= 0) continue;
			Vector3 pos = bManager->bullets[i].pos;
			Vector3 last_pos = bManager->bullets[i].last_pos;
			Vector3 director = pos - last_pos;

			if (sEntity->mesh->collision_model->rayCollision(last_pos.v, director.v, true) == false)continue;
			return true;
			//Llamar a onBulletCollision
		}
	}
	return false;
}

bool CollisionManager::bulletToDynamic()
{
	bManager = BulletMaganer::getInstance();

	for (int i = 0; i < dynamic_entities.size(); i++) {

		EntityCollider* dEntity = dynamic_entities[i];
		dEntity->mesh->collision_model->setTransform(dEntity->getGlobalMatrix().m);

		for (int j = 0; j < MAX_BULLETS; j++) {

			if (bManager->bullets[j].ttl <= 0) continue;
			Vector3 pos = bManager->bullets[j].pos;
			Vector3 last_pos = bManager->bullets[j].last_pos;
			Vector3 director = pos - last_pos;

			if (dEntity->mesh->collision_model->rayCollision(last_pos.v, director.v, true) == false) continue;

			//si destruimos la entidad tenemos que sacarla de este vector
			//dEntity->destroyEntity();
			return true;
		}
	}
	return false;

}

void CollisionManager::setDynamic(EntityCollider* entity)
{
	dynamic_entities.push_back(entity);
}

void CollisionManager::setStatic(EntityCollider* entity)
{
	static_entities.push_back(entity);
}
