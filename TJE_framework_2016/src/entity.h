#ifndef ENTITY_H
#define ENTITY_H


#include "framework.h"
#include "utils.h"

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



class Fighter : public EntityMesh
{
public:
	float speed;
	Vector3 velocity;
	Vector3 camera_info;
	float tta; //time to accelerate
	bool accelerator;

	Fighter();

	void update(float dt);
	void shoot();
	void accelerate();
	void updateCamera(Camera* camera);

	Vector3 getCameraEye();


};


#endif