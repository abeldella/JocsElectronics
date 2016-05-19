#include "mesh.h"
#include <cassert>
#include "includes.h"
#include "shader.h"
#include "extra\textparser.h"

struct sMeshbin {
	char format[4];
	int num_vertices;
	int num_normals;
	int num_uvs;
	Vector3 c;
	Vector3 hS;
};

Mesh::Mesh()
{
	vertices_vbo_id = 0;
	normals_vbo_id = 0;
	uvs_vbo_id = 0;
	colors_vbo_id = 0;

	center = Vector3();
	halfSize = Vector3();
}

Mesh::Mesh( const Mesh& m )
{
	vertices = m.vertices;
	normals = m.normals;
	uvs = m.uvs;
	colors = m.colors;
}

Mesh::~Mesh()
{
	if(vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if(normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if(uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if(colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);
}

void Mesh::clear()
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();
}

void Mesh::render(int primitive)
{
	assert(vertices.size() && "No vertices in this mesh");

    glEnableClientState(GL_VERTEX_ARRAY);

	if(vertices_vbo_id)
	{
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id );
		glVertexPointer(3, GL_FLOAT, 0, NULL );
	}
	else
		glVertexPointer(3, GL_FLOAT, 0, &vertices[0] );

	if (normals.size())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		if(normals_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id );
			glNormalPointer(GL_FLOAT, 0, NULL );
		}
		else
			glNormalPointer(GL_FLOAT, 0, &normals[0] );
	}

	if (uvs.size())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(uvs_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id );
			glTexCoordPointer(2, GL_FLOAT, 0, NULL );
		}
		else
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0] );
	}

	if (colors.size())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if(colors_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id );
			glColorPointer(4, GL_FLOAT, 0, NULL );
		}
		else
			glColorPointer(4, GL_FLOAT, 0, &colors[0] );
	}
    
	glDrawArrays(primitive, 0, (GLsizei)vertices.size() );
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals.size())
		glDisableClientState(GL_NORMAL_ARRAY);
	if (uvs.size())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (colors.size())
		glDisableClientState(GL_COLOR_ARRAY);
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
}

void Mesh::render( int primitive, Shader* sh )
{
	if(!sh || !sh->compiled)
        return render(primitive);
    
	assert(vertices.size() && "No vertices in this mesh");

    int vertex_location = sh->getAttribLocation("a_vertex");
	assert(vertex_location != -1 && "No a_vertex found in shader");

	if(vertex_location == -1)
		return;

    glEnableVertexAttribArray(vertex_location);
	if(vertices_vbo_id)
	{
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id );
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	}
	else
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0] );

	int normal_location = -1;
    if (normals.size())
    {
        normal_location = sh->getAttribLocation("a_normal");
        if(normal_location != -1)
        {
            glEnableVertexAttribArray(normal_location);
			if(normals_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id );
	            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, &normals[0] );
        }
    }
    
	int uv_location = -1;
	if (uvs.size())
	{
        uv_location = sh->getAttribLocation("a_uv");
        if(uv_location != -1)
        {
            glEnableVertexAttribArray(uv_location);
			if(uvs_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id );
	            glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, &uvs[0] );
        }
    }
    
	int color_location = -1;
	if (colors.size())
	{
        color_location = sh->getAttribLocation("a_color");
        if(color_location != -1)
        {
            glEnableVertexAttribArray(color_location);
			if(colors_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id );
	            glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, &colors[0] );
        }
    }
    
	glDrawArrays(primitive, 0, (GLsizei)vertices.size() );

	glDisableVertexAttribArray( vertex_location );
	if(normal_location != -1) glDisableVertexAttribArray( normal_location );
	if(uv_location != -1) glDisableVertexAttribArray( uv_location );
	if(color_location != -1) glDisableVertexAttribArray( color_location );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
}

void Mesh::uploadToVRAM()
{
	//delete old
	if(vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if(normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if(uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if(colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);

	glGenBuffersARB( 1, &vertices_vbo_id ); //generate one handler (id)
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id ); //bind the handler
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float), &vertices[0], GL_STATIC_DRAW_ARB ); //upload data

	if(normals.size())
	{
		glGenBuffersARB( 1, &normals_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, normals.size() * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW_ARB ); //upload data
	}

	if(uvs.size())
	{
		glGenBuffersARB( 1, &uvs_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, uvs.size() * 2 * sizeof(float), &uvs[0], GL_STATIC_DRAW_ARB ); //upload data
	}

	if(colors.size())
	{
		glGenBuffersARB( 1, &colors_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, uvs.size() * 4 * sizeof(float), &colors[0], GL_STATIC_DRAW_ARB ); //upload data
	}

}

void Mesh::createQuad(float center_x, float center_y, float w, float h, bool flip_uvs )
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back( Vector3( center_x + w*0.5f, center_y + h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x + w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y + h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x + w*0.5f, center_y + h*0.5f, 0.0f ) );

	//texture coordinates
	uvs.push_back( Vector2(1.0f,flip_uvs ? 0.0f : 1.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(1.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 0.0f : 1.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(1.0f,flip_uvs ? 0.0f : 1.0f) );

	//all of them have the same normal
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
}

void Mesh::createPlane(float size)
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(size,0,-size) );
	vertices.push_back( Vector3(-size,0,-size) );
	vertices.push_back( Vector3(-size,0,size) );
	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(-size,0,-size) );

	//all of them have the same normal
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );

	//texture coordinates
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(1,0) );
	uvs.push_back( Vector2(0,0) );
	uvs.push_back( Vector2(0,1) );
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(0,0) );

	Vector3 max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < vertices.size(); i++) {
		Vector3 v = vertices[i];
		if (v.x > max.x) { max.x = v.x; }
		if (v.y > max.y) { max.y = v.y; }
		if (v.z > max.z) { max.z = v.z; }
		if (v.x < min.x) { min.x = v.x; }
		if (v.y < min.y) { min.y = v.y; }
		if (v.z < min.z) { min.z = v.z; }
	}

	center = (max + min) * 0.5;
	halfSize = max - center;
	std::cout << "halfSize = " << halfSize.length() << std::endl;
}

bool Mesh::loadASE(const char* filename) {
	/* Al almacenar los vertices se hace en orden contra las agujas del reloj, 
	 * por eso almacenamos en orden (A, C, B) o (x, z, y) */
	
	clear();
	
	//Comprobamos si existe el archivo binario de la mesh, y en ese caso lo cargamos
	std::string bin_filename;
	bin_filename = filename + std::string(".bin");

	if(loadBIN(bin_filename.c_str())){
		std::cout << "Existe BIN" << std::endl;
		return true;
	}

	TextParser t;
	//Cargamos un archivo ASE
	if (!t.create(filename)) {
		std::cout << "File: mesh.cpp - TextParser::create() fail" << std::endl;
		exit(0);
	}

	//Tratamiento del archivo ASE para obtener los valores pertinentes
	t.seek("*MESH_NUMVERTEX");
	int num_vertex = t.getint(); //Numero de vertices de la mesh
	std::cout << "num vertex: " << num_vertex << std::endl;
	t.seek("*MESH_NUMFACES");
	int num_faces = t.getint(); //Numero de caras (triangulos) de la mesh
	std::cout << "num faces: " << num_faces << std::endl;

	//Almacenamos las coordenadas de cada uno de los vertices
	unique_vertices.resize(num_vertex);
	Vector3 max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < num_vertex; i++) {
		t.seek("*MESH_VERTEX");
		t.getint();
		Vector3 v;
		v.x = t.getfloat();
		v.z = t.getfloat() * (-1);
		v.y = t.getfloat();
		unique_vertices[i] = v;

		if (v.x > max.x) { max.x = v.x; }
		if (v.y > max.y) { max.y = v.y; }
		if (v.z > max.z) { max.z = v.z; }
		if (v.x < min.x) { min.x = v.x; }
		if (v.y < min.y) { min.y = v.y; }
		if (v.z < min.z) { min.z = v.z; }
	}
	center = (max + min) * 0.5;
	halfSize = max - center;
	std::cout << "halfSize = " << halfSize.length() << std::endl;
	
	//Almacenamos los aristas existentes en la mesh entre los vertices
	t.seek("*MESH_FACE_LIST");
	vertices.resize(num_faces*3);
	for (int i = 0; i < num_faces*3; i = i+3) {
		t.seek("*MESH_FACE");
		t.getword();
		t.getword();

		int A = t.getint();
		t.getword();
		int B = t.getint();
		t.getword();
		int C = t.getint();
		vertices[i] = unique_vertices[A];
		vertices[i+1] = unique_vertices[B];
		vertices[i+2] = unique_vertices[C];
	}

	t.seek("*MESH_NUMTVERTEX");
	int numtvertex = t.getint(); //Numero de vertices de la textura
	std::cout << "num TVERTEX: " << numtvertex << std::endl;
	
	//Almacenamos las coordenadas en 2D de cada vertice en la textura
	unique_uvs.resize(numtvertex);
	for (int i = 0; i < numtvertex; i++) {
		t.seek("*MESH_TVERT");
		t.getint();
		Vector2 v;
		v.x = t.getfloat();
		v.y = t.getfloat();
		unique_uvs[i] = v;
	}

	t.seek("*MESH_NUMTVFACES");
	int num_vfaces = t.getint(); //Numero de caras de la textura
	
	//Almacenamos las aristas que forman cada cara de la textura
	uvs.resize(num_vfaces*3);
	for (int i = 0; i < num_vfaces*3; i = i+3) {
		t.seek("*MESH_TFACE");
		t.getint(); 
		int A = t.getint();
		int B = t.getint();
		int C = t.getint();
		uvs[i] = unique_uvs[A];
		uvs[i+1] = unique_uvs[B];
		uvs[i+2] = unique_uvs[C];
	}

	//Almacenamos las normales de la mesh
	t.seek("*MESH_NORMALS");
	normals.resize(num_faces);
	for (int i = 0; i < num_faces; i++) {
		for (int j = 0; j < 3; j++) {
			t.seek("*MESH_VERTEXNORMAL");
			t.getint();
			Vector3 v;
			v.x = t.getfloat();
			v.z = t.getfloat() * (-1);
			v.y = t.getfloat();
			normals[i] = v;
		}
	}
	writeBIN(bin_filename.c_str());
	return true;
}

bool Mesh::writeBIN(const char* filename) {
	std::cout << "Creando BIN " << filename << std::endl;
	sMeshbin header;
	header.num_vertices = vertices.size();
	header.num_normals = normals.size();
	header.num_uvs = uvs.size();
	header.format[0] = 'M';
	header.format[1] = 'E';
	header.format[2] = 'S';
	header.format[3] = 'H';
	header.c = center;
	header.hS = halfSize;
	
	FILE* f = fopen(filename, "wb");
	if (f == NULL) {
		std::cout << "writeBIN()::Error al abrir archivo" << std::endl;
		return false;
	}
	fwrite(&header, sizeof(sMeshbin), 1, f);
	fwrite(&vertices[0], sizeof(Vector3), vertices.size(), f);
	if (normals.size()) {
		fwrite(&normals[0], sizeof(Vector3), normals.size(), f);
	}
	if (uvs.size()) {
		fwrite(&uvs[0], sizeof(Vector2), uvs.size(), f);
	}	
	fclose(f);

	return true;
}

bool Mesh::loadBIN(const char* filename) {
	sMeshbin header;

	FILE* f = fopen(filename, "rb");
	if (f == NULL) {
		std::cout << "loadBIN()::Error al abrir archivo" << std::endl;
		return false;
	}
	fread(&header, sizeof(sMeshbin), 1, f);
	vertices.resize(header.num_vertices);
	fread(&vertices[0], sizeof(Vector3), header.num_vertices, f);
	if (header.num_normals) {
		normals.resize(header.num_normals);
		fread(&normals[0], sizeof(Vector3), header.num_normals, f);
	}
	if (header.num_uvs) {
		uvs.resize(header.num_uvs);
		fread(&uvs[0], sizeof(Vector2), header.num_uvs, f);
	}
	center = header.c;
	halfSize = header.hS;
	fclose(f);

	return true;
}


bool Mesh::loadOBJ(const char* filename)
{
	std::string bin_filename;
	bin_filename = filename + std::string(".bin");

	if (loadBIN(bin_filename.c_str())) {
		std::cout << "Existe BIN" << std::endl;
		return true;
	}

	struct stat stbuffer;

	FILE* f = fopen(filename, "rb");
	if (f == NULL)
	{
		std::cerr << "File not found: " << filename << std::endl;
		return false;
	}

	stat(filename, &stbuffer);

	unsigned int size = stbuffer.st_size;
	char* data = new char[size + 1];
	fread(data, size, 1, f);
	fclose(f);
	data[size] = 0;

	char* pos = data;
	char line[255];
	int i = 0;

	std::vector<Vector3> indexed_positions;
	std::vector<Vector3> indexed_normals;
	std::vector<Vector2> indexed_uvs;

	const float max_float = 10000000;
	const float min_float = -10000000;

	unsigned int vertex_i = 0;

	//parse file
	while (*pos != 0)
	{
		if (*pos == '\n') pos++;
		if (*pos == '\r') pos++;

		//read one line
		i = 0;
		while (i < 255 && pos[i] != '\n' && pos[i] != '\r' && pos[i] != 0) i++;
		memcpy(line, pos, i);
		line[i] = 0;
		pos = pos + i;

		//std::cout << "Line: \"" << line << "\"" << std::endl;
		if (*line == '#' || *line == 0) continue; //comment

												  //tokenize line
		std::vector<std::string> tokens = tokenize(line, " ");

		if (tokens.empty()) continue;

		if (tokens[0] == "v" && tokens.size() == 4)
		{
			Vector3 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
			indexed_positions.push_back(v);
		}
		else if (tokens[0] == "vt" && tokens.size() == 4)
		{
			Vector2 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()));
			indexed_uvs.push_back(v);
		}
		else if (tokens[0] == "vn" && tokens.size() == 4)
		{
			Vector3 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
			indexed_normals.push_back(v);
		}
		else if (tokens[0] == "s") //surface? it appears one time before the faces
		{
			//process mesh
			if (uvs.size() == 0 && indexed_uvs.size())
				uvs.resize(1);
		}
		else if (tokens[0] == "f" && tokens.size() >= 4)
		{
			Vector3 v1, v2, v3;
			v1 = parseVector3(tokens[1].c_str(), '/');

			for (int iPoly = 2; iPoly < tokens.size() - 1; iPoly++)
			{
				v2 = parseVector3(tokens[iPoly].c_str(), '/');
				v3 = parseVector3(tokens[iPoly + 1].c_str(), '/');

				vertices.push_back(indexed_positions[unsigned int(v1.x) - 1]);
				vertices.push_back(indexed_positions[unsigned int(v2.x) - 1]);
				vertices.push_back(indexed_positions[unsigned int(v3.x) - 1]);
				//triangles.push_back( VECTOR_INDICES_TYPE(vertex_i, vertex_i+1, vertex_i+2) ); //not needed
				vertex_i += 3;

				if (indexed_uvs.size() > 0)
				{
					uvs.push_back(indexed_uvs[unsigned int(v1.y) - 1]);
					uvs.push_back(indexed_uvs[unsigned int(v2.y) - 1]);
					uvs.push_back(indexed_uvs[unsigned int(v3.y) - 1]);
				}

				if (indexed_normals.size() > 0)
				{
					normals.push_back(indexed_normals[unsigned int(v1.z) - 1]);
					normals.push_back(indexed_normals[unsigned int(v2.z) - 1]);
					normals.push_back(indexed_normals[unsigned int(v3.z) - 1]);
				}
			}
		}
	}

	Vector3 max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < vertices.size(); i++) {
		Vector3 v = vertices[i];
		if (v.x > max.x) { max.x = v.x; }
		if (v.y > max.y) { max.y = v.y; }
		if (v.z > max.z) { max.z = v.z; }
		if (v.x < min.x) { min.x = v.x; }
		if (v.y < min.y) { min.y = v.y; }
		if (v.z < min.z) { min.z = v.z; }
	}

	center = (max + min) * 0.5;
	halfSize = max - center;
	std::cout << "halfSize = " << halfSize.length() << std::endl;

	writeBIN(bin_filename.c_str());
	return true;
}

std::vector<std::string> Mesh::tokenize(const std::string& source, const char* delimiters, bool process_strings)
{
	std::vector<std::string> tokens;

	std::string str;
	char del_size = strlen(delimiters);
	const char* pos = source.c_str();
	char in_string = 0;
	int i = 0;
	while (*pos != 0)
	{
		bool split = false;

		if (!process_strings || (process_strings && in_string == 0))
		{
			for (i = 0; i < del_size && *pos != delimiters[i]; i++);
			if (i != del_size) split = true;
		}

		if (process_strings && (*pos == '\"' || *pos == '\''))
		{
			if (!str.empty() && in_string == 0) //some chars remaining
			{
				tokens.push_back(str);
				str.clear();
			}

			in_string = (in_string != 0 ? 0 : *pos);
			if (in_string == 0)
			{
				str += *pos;
				split = true;
			}
		}

		if (split)
		{
			if (!str.empty())
			{
				tokens.push_back(str);
				str.clear();
			}
		}
		else
			str += *pos;
		pos++;
	}
	if (!str.empty())
		tokens.push_back(str);
	return tokens;
}

Vector3 Mesh::parseVector3(const char* text, const char separator)
{
	int pos = 0;
	char num[255];
	const char* start = text;
	const char* current = text;
	Vector3 result;

	while (1)
	{
		if (*current == separator || (*current == '\0' && current != text))
		{
			strncpy(num, start, current - start);
			num[current - start] = '\0';
			start = current + 1;
			if (num[0] != 'x') //¿?
				switch (pos)
				{
				case 0: result.x = (float)atof(num); break;
				case 1: result.y = (float)atof(num); break;
				case 2: result.z = (float)atof(num); break;
				default: return result; break;
				}

			++pos;
			if (*current == '\0')
				break;
		}

		++current;
	}

	return result;
};


MeshManager* MeshManager::instance = NULL;

MeshManager::MeshManager() {
	assert(instance == NULL); //must be only one
	instance = this;
}

Mesh* MeshManager::getMesh(const char* filename) {
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;
	Mesh* mesh = new Mesh();

	std::string str = filename;
	std::string ext = str.substr(str.size() - 4, 4);
	bool found;

	if (ext == ".obj" || ext == ".OBJ")
	{
		found = mesh->loadOBJ(filename);
	}
	else {
		found = mesh->loadASE(filename);
	}
	
	if (!found) {
		std::cout << "Mesh not found: " << filename << std::endl;
		assert(!"file not found");
		delete mesh;
		return NULL;
	}
	mesh->uploadToVRAM();
	s_map[filename] = mesh;
	return mesh;
}