#ifndef BULLETS_H
#define BULLETS_H

#include "framework.h"
#define MAX_BULLETS 1024
#define MAX_TTL 5

class Camera;

class Bullet
{
public:
	Vector3 pos;
	Vector3 vel;
	Vector3 last_pos;
	char type;
	void* owner;
	float ttl;

};

class BulletMaganer
{
private:
	Bullet bullets[MAX_BULLETS];
	BulletMaganer();
public:
	
	static BulletMaganer* instance;
	static BulletMaganer* getInstance() {
		if (instance == NULL)
			instance = new BulletMaganer();
		return instance;
	}


	void createBullet(Vector3 pos, Vector3 vel, float ttl, void* owner);
	void render(Camera* camera);
	void update(float dt);

};

#endif 