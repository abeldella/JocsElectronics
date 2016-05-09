#include "bullets.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"


BulletMaganer* BulletMaganer::instance = NULL;

BulletMaganer::BulletMaganer() 
{
	assert(instance == NULL); //must be only one
	instance = this;
	memset(bullets, 0, sizeof(Bullet) * MAX_BULLETS);
}

void BulletMaganer::createBullet(Vector3 pos, Vector3 vel, float ttl, void* owner)
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		
		Bullet& bullet = bullets[i];
		if (bullet.ttl > 0)
			continue;

		bullet.pos = pos;
		bullet.vel = vel;
		bullet.ttl = ttl;
		bullet.owner = owner;

		return;
	}
}

void BulletMaganer::render(Camera* camera)
{
	Mesh mesh;
	for (int i = 0; i < MAX_BULLETS; i++) {

		Bullet& bullet = bullets[i];
		if (bullet.ttl <= 0)
			continue;
		mesh.vertices.push_back(bullet.last_pos);
		mesh.vertices.push_back(bullet.pos);
		mesh.colors.push_back(Vector4(0, 0, 0, 1));
		mesh.colors.push_back(Vector4(1, 1, 0.3, 1));

	}

	if (mesh.vertices.size() == 0)
		return;

	glLineWidth(3);
	mesh.render(GL_LINES);
}

void BulletMaganer::update(float dt)
{
	for (int i = 0; i < MAX_BULLETS; i++) {

		Bullet& bullet = bullets[i];
		if (bullet.ttl < 0)
			continue;
		bullet.last_pos = bullet.pos;
		bullet.pos = bullet.pos + bullet.vel * dt;
		bullet.ttl -= dt;
	}
}
