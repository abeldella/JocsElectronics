
#include "manager.h"
#include "mesh.h"
#include "entity.h"
#include "camera.h"
#include "game.h"

#include "extra\coldet\coldet.h"


CollisionManager* CollisionManager::instance = NULL;

CollisionManager::CollisionManager()
{
	assert(instance == NULL); //must be only one
	instance = this;

}

void CollisionManager::update(float dt)
{
	if (bulletToStatic()) //std::cout << "BulletToStatic Colisiona!!!" << std::endl;
	if (bulletToDynamic()) std::cout << "BulletToDynamic Colisiona!!! Entity destroyed!" << std::endl;

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
			//bManager->bullets[j].ttl = 0;
			//sEntity->onBulletCollision();
			return true;
		}
	}
	return false;
}

bool CollisionManager::bulletToDynamic()
{
	bManager = BulletMaganer::getInstance();
	//IDEA: poner un vector de destriubles para evitar eliminar cosas mientras se itera 

	for (int i = 0; i < dynamic_entities.size(); i++) {

		EntityCollider* dEntity = dynamic_entities[i];
		dEntity->mesh->collision_model->setTransform(dEntity->getGlobalMatrix().m);

		for (int j = 0; j < MAX_BULLETS; j++) {

			if (bManager->bullets[j].ttl <= 0) continue;
			if (dEntity == bManager->bullets[j].owner)continue;

			Vector3 pos = bManager->bullets[j].pos;
			Vector3 last_pos = bManager->bullets[j].last_pos;
			Vector3 director = pos - last_pos;

			//Matrix44 global = dEntity->getGlobalMatrix();
			//Vector3 center = global * dEntity->mesh->center;
			//Vector3 point;
			//float radius = dEntity->mesh->halfSize.length();
			//if ( SphereRayCollision(center.v, radius, last_pos.v, pos.v, point.v ) == false)continue;

			if (dEntity->mesh->collision_model->rayCollision(last_pos.v, director.v, true) == false) continue;

			if (!dEntity->onBulletCollision()) return false;
			
			dynamic_entities.erase(dynamic_entities.begin() + i);
			return true; //solo devolvemos true en caso de destruir la entidad
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
				//Desplazamiento en caso de choque PRUEBA
				Vector3 front = dEntity->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
				Vector3 position = dEntity->global_matrix.getTranslation();
	
				int x, y, z, d = 5;
				float angle = 0.6;
				if (position.x < 0) {
					x = d;
					angle = -angle;
				}
				else x = -d;

				if (position.y < 10) y = d;
				else if (position.y > 380) y = -d;
				else y = 0;

				if (position.z < 0) {
					z = d;
					angle = -angle;
				}
				else z = -d;

				dEntity->local_matrix.setTranslation(position.x + x, position.y + y, position.z + z);
				dEntity->local_matrix.rotateLocal(angle, front);

				if (!dEntity->onCollision()) return false;
				dynamic_entities.erase(dynamic_entities.begin() + i);
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
				Vector3 front1 = dEntity1->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
				Vector3 front2 = dEntity2->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
				Vector3 position1 = dEntity1->global_matrix.getTranslation();
				Vector3 position2 = dEntity2->global_matrix.getTranslation();

				dEntity1->local_matrix.setTranslation(position1.x - 5, position1.y - 5, position1.z - 5);
				dEntity1->local_matrix.rotateLocal(0.60, front1);
				dEntity2->local_matrix.setTranslation(position2.x - 5, position2.y - 5, position2.z - 5);
				dEntity2->local_matrix.rotateLocal(0.60, front2);

				bool return1, return2;
				return1 = return2 = true;
				if (!dEntity1->onCollision()) return1 = false;		
				if (!dEntity2->onCollision()) return2 = false;
				
				if(return1)dynamic_entities.erase(dynamic_entities.begin() + i);
				if(return2)dynamic_entities.erase(dynamic_entities.begin() + j);
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
