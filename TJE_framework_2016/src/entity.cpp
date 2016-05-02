#include "entity.h"

#include "camera.h"
#include "mesh.h"
#include "texture.h"

Entity::Entity() 
{
	parent = NULL;

}

Entity::~Entity() 
{
	std::cout << "Entity destroyed" << std::endl;
	//To do 
	//Avisar al padre para que borre este hijo

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
	local_matrix.setRotation(dt, Vector3(0, 1, 0));

	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}





EntityMesh::EntityMesh() 
{
	mesh = lod_mesh = NULL;
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
			
			if (texture)
				texture->bind();
			if (two_sided)
				glDisable(GL_CULL_FACE);
			glPushMatrix();
			global_matrix.multGL();
			Mesh* render_mesh = mesh;

			if (lod_mesh && pos.distance(camera->eye) > render_halfSize * 10 ) {
				render_mesh = lod_mesh;
			}
			render_mesh->render(GL_TRIANGLES);
			if (two_sided)
				glEnable(GL_CULL_FACE);
			if (texture)
				texture->unbind();
			glPopMatrix();

		}

	}
	//propagate
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(camera);
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