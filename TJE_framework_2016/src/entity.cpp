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
	//To do 
	//Avisar al padre para que borre este hijo

}

void Entity::addChildren(Entity* entity)
{
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



EntityMesh::EntityMesh() 
{
	mesh = lod_mesh = NULL;
	texture = NULL;
}

void EntityMesh::render(Camera* camera) 
{
	
	if (mesh) {

		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 pos = global_matrix.getTranslation();
		float render_halfSize;
		
		render_halfSize = mesh->halfSize.length();

		if (camera->testSphereInFrustum(pos + mesh->center, render_halfSize)) {
			
			if (texture)
				texture->bind();
			glPushMatrix();
			global_matrix.multGL();
			Mesh* render_mesh = mesh;

			if (lod_mesh && pos.distance(camera->eye) > render_halfSize * 10 ) {
				render_mesh = lod_mesh;
			}
			render_mesh->render(GL_TRIANGLES);
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