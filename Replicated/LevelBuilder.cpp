#include "LevelBuilder.h"

LevelBuilder::LevelBuilder()
{
	InitialiseAssets();
}

LevelBuilder::~LevelBuilder()
{
	delete cubeMesh;

	delete basicTex;

	delete basicShader;
}

void LevelBuilder::BuildLevel()
{
	/*
	TODO:

	Set what the current game world is (on server?)

	Create the functions to load the objects into that game world
	AddCubeToWorld
	AddSphereToWorld
	AddCapsuleToWorld

	Create a level reader object

	read in the current level from the JSON file
	using the ReadLevel() function on the level reader
	
	This will give a level reader object that has
	Vector3 of start pos
	Vector3 of end pos
	vector of groundcubeprimative objects
		each groundcubeprimvative has a pos and dim

	These will be used to create the cubes in the level

	*/
}

void LevelBuilder::InitialiseAssets()
{
	// initialise the meshes, textures and shaders here
	// This may be moved elsewhere, later in the project.
	// Currently it is here, as the assets need to be initialised for the
	// add cube to world function
}

GameObject* LevelBuilder::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, GameWorld* world)
{
	return nullptr;
}
