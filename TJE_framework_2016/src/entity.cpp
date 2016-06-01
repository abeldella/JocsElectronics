#include "entity.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "bullets.h"
#include "game.h"
#include "manager.h"

unsigned int Entity::numEntidades = 0;
std::vector<Entity*> Entity::to_destroy;

Entity::Entity() 
{
	parent = NULL;
	destroy_entity = false;
	uid = numEntidades;
	numEntidades++;
	ttd = 3;
}

Entity::~Entity() 
{
	std::cout << "Entity destroyed" << std::endl;
	//To do 
	//Al destruir la entidad eliminamos todos sus hijos
	std::vector<Entity*>::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		delete(*it);
	}
}

void Entity::removeChild(Entity * entity) {
	//Eliminamos la entidad del vector de hijos
	for (int i = 0; i < this->children.size(); i++) {
		if (this->children[i] == entity) {
			this->children.erase(this->children.begin() + i);
		}
	}
	//Desvinculamos la entidad de su padre
	entity->parent = NULL;
}

void Entity::destroyChild(Entity * entity) {
	this->removeChild(entity);
	to_destroy.push_back(entity);
}


void Entity::addChildren(Entity* entity)
{
	entity->parent = this;
	children.push_back( entity );
}

void Entity::render( Camera* camera ) 
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(camera);
	}
}

void Entity::update(float dt) 
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
		if(children[i]->destroy_entity)
			children[i]->ttd -= dt;

		if (children[i]->destroy_entity && children[i]->ttd <= 0) {
			destroyChild(children[i]);
		}
	}

	Entity* entity;
	for (int i = 0; i < to_destroy.size(); i++){
		entity = to_destroy[i];
		to_destroy.erase(to_destroy.begin() + i);
		delete(entity);
		}
}

void Entity::move(Vector3 v)
{
	local_matrix.traslateLocal(v.x, v.y, v.z);
}


void Entity::rotate(float angle_in_deg, Vector3 v)
{

	local_matrix.rotateLocal(angle_in_deg * DEG2RAD, v);
}

Matrix44 Entity::getGlobalMatrix() 
{
	if (parent)
		global_matrix = local_matrix * parent->getGlobalMatrix();
	else
		global_matrix = local_matrix;
	return global_matrix;
}

void Entity::destroyEntity()
{
	destroy_entity = true;
}

//---------------------------------------------------------------------------------------------------------
EntityMesh::EntityMesh() 
{
	mesh = lod_mesh = NULL;
	shader = NULL;
	texture = NULL;
	frustum_test = true;
	two_sided = false;
}

EntityMesh::~EntityMesh()
{
	std::cout << "EntityMesh destroyed" << std::endl;
}

void EntityMesh::render(Camera* camera) 
{
	
	if (mesh) {

		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 pos = global_matrix.getTranslation();
		float render_halfSize;
		
		render_halfSize = mesh->halfSize.length();

		if ( !frustum_test || camera->testSphereInFrustum(pos + mesh->center, render_halfSize) ) {
			
			
			if (two_sided)
				glDisable(GL_CULL_FACE);
			
			
			Mesh* render_mesh = mesh;

			if (lod_mesh && pos.distance(camera->eye) > render_halfSize * 10 ) {
				render_mesh = lod_mesh;
			}

			if (shader) {
				Matrix44 mvp = global_matrix * camera->viewprojection_matrix;
				shader->enable();
				shader->setMatrix44("u_model", global_matrix);
				shader->setMatrix44("u_mvp", mvp);
				shader->setTexture("u_texture", texture);
				shader->setVector3("u_camera_eye", camera->eye);
				//shader->setVector4("u_color", color);
				mesh->render(GL_TRIANGLES, shader);
				shader->disable();
			}
			else {
				glPushMatrix();
				if (texture)
					texture->bind();
				global_matrix.multGL();
				render_mesh->render(GL_TRIANGLES);
				if (two_sided)
					glEnable(GL_CULL_FACE);
				if (texture)
					texture->unbind();
				glPopMatrix();
			}
		}

	}
	//propagate
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(camera);
	}
}

void EntityMesh::update(float dt)
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

void EntityMesh::setup(const char* mesh, const char* texture, const char* lod_mesh)
{
	this->mesh = Mesh::get( mesh );
	if (lod_mesh) {
		this->lod_mesh = Mesh::get( lod_mesh );
	}
	if (texture) {
		this->texture = Texture::get( texture );
	}
}


//---------------------------------------------------------------------------------------------------------
EntityCollider::EntityCollider()
{
	dynamic_entity = false;
}

void EntityCollider::onDemand()
{
	mesh->createCollisionModel();

	CollisionManager* manager = CollisionManager::getInstance();
	if (dynamic_entity) {
		manager->setDynamic(this);
	}
	else manager->setStatic(this);
}

void EntityCollider::onBulletCollision()
{
	destroyEntity();
}

//---------------------------------------------------------------------------------------------------------
Fighter::Fighter()
{
	speed = 0.5;	
	tta = 5;
	tts = MAX_TTS;
	camera_info.set(0, 0, 0);
	accelerator = false;

	dynamic_entity = true;

	camera_eye = Vector3(0, 2, -5);
	camera_center = Vector3(0, 0, 10);
}

void Fighter::update(float dt)
{
	//move(Vector3(0, 0, speed * dt));
	local_matrix.traslate(velocity.x, velocity.y, velocity.z);
	//Hacia donde empuja el avion
	Vector3 impulse = global_matrix.rotateVector(Vector3(0, 0, speed));
	velocity = velocity + impulse * dt;
	//Gravedad 
	velocity = velocity + Vector3(0, -0.05 * dt, 0);

	Vector3 pos = local_matrix.getTranslation();
	if (pos.y < 0) {
		local_matrix.m[13] = 0;
	}
	velocity = velocity - velocity * 0.006;

	//Acceleration control
	if (accelerator) {
		tta -= dt;
		if (tta <= 0) {
			speed = 0.5;
			camera_info.z = 0;
			accelerator = false;
		}
	}

	if( tts > 0) 
		tts -= dt;

}


void Fighter::shoot()
{
	if (tts <= 0) {
		BulletMaganer* bulletMng = BulletMaganer::getInstance();
		Matrix44 global_matrix = getGlobalMatrix();
		//Falta tener en cuenta la velocidad del avion
		Vector3 vel = global_matrix.rotateVector(Vector3(0, 0, 1000));
		Vector3 pos = global_matrix * Vector3(2.1, -0.55, 0.6);
		pos = pos + velocity;
		bulletMng->createBullet(pos, vel, MAX_TTL, this);
		pos = global_matrix * Vector3(-2.1, -0.55, 0.6);
		pos = pos + velocity;
		bulletMng->createBullet(pos, vel, MAX_TTL, this);

		tts = MAX_TTS;
	}

}

Vector3 Fighter::getCameraEye()
{
	return camera_info;
}

//ESTA FUNCION DEBE DE IR EN EL CONTROLLER
void Fighter::updateCamera(Camera* camera)
{
	Matrix44 rot;
	rot.rotateLocal(camera_info.x * 2.0, Vector3(0, 1, 0));
	Matrix44 global = getGlobalMatrix();
	Vector3 eye = global * (rot * camera_eye);
	Vector3 center = global * (rot * camera_center);
	/*
	Vector3 eye = global * (rot * Vector3(0, 8, -20));
	Vector3 center = global * (rot * Vector3(0, 9, 30)); */
	Vector3 up = global.rotateVector(Vector3(0, 1, 0));

	camera->setPerspective(70 + camera_info.z * 20, Game::instance->window_width / (float)Game::instance->window_height, 0.1, 25000);
	camera->lookAt(eye, center, up);
}

void Fighter::accelerate()
{
	speed = 5;
	tta = 10;
	//camera_info.z = 0.9;
	accelerator = true;
}