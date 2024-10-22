/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	The Mesh contains the info about how to render a mesh and also how to parse it from a file.
*/

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "framework.h"
#include <map>
#include <string>
#include <assert.h>
#include "extra\coldet\coldet.h"

class Shader;
class Mesh;

//MESHMANAGER CLASS
class MeshManager {
public:
	static MeshManager* instance;
	static MeshManager* getInstance() {
		if (instance == NULL)
			instance = new MeshManager();
		return instance;
	}
	Mesh* getMesh(const char* filename);
private:
	MeshManager();
	std::map<std::string, Mesh*> s_map;
};

//MESH CLASS
class Mesh
{
public:
	std::vector< Vector3 > vertices; //here we store the vertices
	std::vector< Vector3 > unique_vertices; //here we store the index of vertices
	std::vector< Vector3 > normals;	 //here we store the normals
	std::vector< Vector2 > uvs;	 //here we store the texture coordinates
	std::vector< Vector2 > unique_uvs;	 //here we store the texture coordinates
	std::vector< Vector4 > colors;	 //here we store colors by vertex
	
	Vector3 center;
	Vector3 halfSize;

	CollisionModel3D* collision_model;

	unsigned int vertices_vbo_id;
	unsigned int normals_vbo_id;
	unsigned int uvs_vbo_id;
	unsigned int colors_vbo_id;

	Mesh();
	Mesh( const Mesh& m );
	~Mesh();

	void clear();
	void render(int primitive);
	void render(int primitive, Shader* sh);

	void uploadToVRAM(); //store the mesh info in the GPU RAM so it renders faster

	void createPlane( float size );
	void createQuad(float center_x, float center_y, float w, float h, bool flip_uvs = false);

	bool loadASE(const char* filename); //Carga un archivo ASE para inicializar los vectores de la mesh
	bool writeBIN(const char* filename);
	bool loadBIN(const char* filename);
	bool loadOBJ(const char* filename);

	void createCollisionModel();

	std::vector<std::string> tokenize(const std::string& source, const char* delimiters, bool process_strings = NULL);
	Vector3 parseVector3(const char* text, const char separator);

	static Mesh* get(const char* filename) {
		return MeshManager::getInstance()->getMesh(filename);
	}
};

#endif