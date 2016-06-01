#ifndef MANAGER_H
#define MANAGER_H

#include "framework.h"
#include "bullets.h"
#include "mesh.h"

class EntityCollider;

//COLLISIONMANAGER CLASS
class CollisionManager 
{
public:
	static CollisionManager* instance;
	static CollisionManager* getInstance() {
		if (instance == NULL)
			instance = new CollisionManager();
		return instance;
	}

	void update(float dt);
	bool dynamicToStatic();
	bool dynamicTodynamic();
	bool bulletToStatic();
	bool bulletToDynamic();

	void setDynamic(EntityCollider* entity);
	void setStatic(EntityCollider* entity);

	Mesh pointsOfCollision;

private:
	CollisionManager();
	//vector dinamico y estatico
	std::vector< EntityCollider* > static_entities;
	std::vector< EntityCollider* > dynamic_entities;

	BulletMaganer* bManager;
};


#endif	