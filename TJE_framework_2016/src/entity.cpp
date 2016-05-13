#include "entity.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "bullets.h"

Entity::Entity() 
{
	parent = NULL;

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

void removeChild(Entity * entity) {

}

void destroyChild(Entity * entity, float time) {

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


Fighter::Fighter()
{
	speed = 1;	
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

}


void Fighter::shoot()
{
	BulletMaganer* bulletMng = BulletMaganer::getInstance();
	Matrix44 global_matrix = getGlobalMatrix();
	//Falta tener en cuenta la velocidad del avion
	Vector3 vel = global_matrix.rotateVector(Vector3(0, 0, 1000));
	Vector3 pos = global_matrix * Vector3(2.1, -0.55, 0.6);

	bulletMng->createBullet(pos, vel, MAX_TTL, this);
	pos = global_matrix * Vector3(-2.1, -0.55, 0.6);
	bulletMng->createBullet(pos, vel, MAX_TTL, this);
}