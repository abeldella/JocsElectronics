#include "world.h"
#include "includes.h"
#include "entity.h"
#include "extra/textparser.h"


#define SPIRTFIRE 1

World* World::instance = NULL;

World::World()
{
	root = NULL;
	skybox = NULL;
}


Entity* World::factory(const char* filename)
{

	TextParser t;
	//Cargamos un archivo ASE
	if (!t.create(filename)) {
		std::cout << "File: world.cpp - TextParser::create() fail" << std::endl;
		exit(0);
	}
	
	return NULL;
}

