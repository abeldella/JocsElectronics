
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

	if (dynamicToStatic()) std::cout << "DynamicToStatic Colisiona!!!" << std::endl;
	if (dynamicTodynamic()) std::cout << "DynamicToDynamic Colisiona!!!" << std::endl;
}

bool CollisionManager::bulletToStatic()
{
	bManager = BulletMaganer::getInstance();

	for (int i = 0; i < static_entities.size(); i++) {

		EntityCollider* sEntity = static_entities[i];
		sEntity->mesh->collision_model->setTransform(sEntity->getGlobalMatrix().m);

		for (int j = 0; j < MAX_BULLETS; j++) {	
			
			if (bManager->bullets[j].ttl <= 0) continue;
			Vector3 pos = bManager->bullets[j].pos;
			Vector3 last_pos = bManager->bullets[j].last_pos;
			Vector3 director = pos - last_pos;

			if (sEntity->mesh->collision_model->rayCollision(last_pos.v, director.v, true) == false)continue;

			Vector3 collision;
			sEntity->mesh->collision_model->getCollisionPoint(collision.v, true);
			
			collision = sEntity->getGlobalMatrix() * collision;
			pointsOfCollision.vertices.push_back(collision);

			//static_entities.erase(static_entities.begin() + i);
			bManager->bullets[j].ttl = 0;
			//sEntity->onBulletCollision();
			return true;
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
			if (dEntity == bManager->bullets[j].owner)continue;

			Vector3 pos = bManager->bullets[j].pos;
			Vector3 last_pos = bManager->bullets[j].last_pos;
			Vector3 director = pos - last_pos;

			if (dEntity->mesh->collision_model->rayCollision(last_pos.v, director.v, true) == false) continue;

			//si destruimos la entidad tenemos que sacarla del vector de entidades dinamicas
			dynamic_entities.erase(dynamic_entities.begin() + i);

			dEntity->onBulletCollision();
			return true;
		}
	}
	return false;

}

bool CollisionManager::dynamicToStatic()
{
	for (int i = 0; i < dynamic_entities.size(); i++) {
		for (int j = 0; j < static_entities.size(); j++) {

			EntityCollider* dEntity = dynamic_entities[i];
			EntityCollider* sEntity = static_entities[j];

			dEntity->mesh->collision_model->setTransform(dEntity->getGlobalMatrix().m);

			bool test = dEntity->mesh->collision_model->collision(sEntity->mesh->collision_model, -1, 0, sEntity->getGlobalMatrix().m);
			
			if (test) {
				return true;
			}
		}
	}
	return false;
}

bool CollisionManager::dynamicTodynamic()
{
	for (int i = 0; i < dynamic_entities.size(); i++) {
		for (int j = i+1; j < dynamic_entities.size(); j++) {

			EntityCollider* dEntity1 = dynamic_entities[i];
			EntityCollider* dEntity2 = dynamic_entities[j];

			if (dEntity1 == player_entity)continue;

			dEntity1->mesh->collision_model->setTransform(dEntity1->getGlobalMatrix().m);

			bool test = dEntity1->mesh->collision_model->collision(dEntity2->mesh->collision_model, -1, 0, dEntity2->getGlobalMatrix().m);

			if (test) {
				return true;
			}
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
