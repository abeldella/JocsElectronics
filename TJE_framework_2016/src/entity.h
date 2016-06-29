#ifndef ENTITY_H
#define ENTITY_H


#include "framework.h"
#include "utils.h"

#define MAX_TTS 0.1
#define MAX_DAMAGE 10

class Camera;
class Mesh;
class Texture;
class Shader;

class Entity
{
public:
	static unsigned int numEntidades;
	unsigned int uid;
	std::string name;
	Matrix44 local_matrix;
	Matrix44 global_matrix;
	std::vector<Entity*> children;
	static std::vector<Entity*> to_destroy;
	Entity* parent;
	float ttd; //time to destroy
	bool destroy_entity; //setting entity to destroy

	Entity();
	virtual ~Entity();

	virtual void render( Camera* camera );
	virtual void update(float dt);

	//Este metodo es para mover un objeto quiza no encaja en entity sino en entity mesh
	void move(Vector3 v);
	void rotate(float angle_in_deg, Vector3 v);

	void addChildren(Entity* entity);
	void removeChild(Entity* entity); //Desenlaza una entidad de su padre
	void destroyChild(Entity* entity); //Destruye una entidad y sus hijos

	void destroyEntity();

	Matrix44 getGlobalMatrix();

};

class EntityMesh : public Entity
{
public:
	Mesh* mesh;
	Mesh* lod_mesh;
	Texture* texture;
	Shader* shader;

	bool two_sided;
	bool frustum_test;

	EntityMesh();
	virtual ~EntityMesh();
	
	void render( Camera* camera );
	void setup(const char* mesh, const char* texture = NULL, const char* lod_mesh = NULL);
	void update(float dt);

};

class EntityCollider : public EntityMesh
{
public:
	bool dynamic_entity;

	EntityCollider();
	virtual void onDemand();
	virtual bool onBulletCollision();
	virtual bool onCollision();

};


class Fighter : public EntityCollider
{
public:
	Vector3 velocity;
	Vector3 camera_info;

	Vector3 camera_eye;
	Vector3 camera_center;

	float tta; //time to accelerate
	float tts; //time between shoot
	float ttsc; //time between shoot custom
	float start;

	int life;

	float speed;
	float speedc;
	bool accelerator;

	Fighter();
	virtual ~Fighter();

	void update(float dt);
	virtual void shoot();
	void accelerate();
	void updateCamera(Camera* camera);

	void setTimetoShoot(float time);
	void setSpeed(float velocity);

	virtual bool onBulletCollision();
	virtual bool onCollision();

	Vector3 getCameraEye();
	static std::vector<Fighter*> s_fighters;
};

class AntiAircraft : public Fighter
{
public:

	AntiAircraft();

	void shoot();
	void update(float dt);
};

#endif