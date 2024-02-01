#pragma once
#include "GameObject.h"
#include "Vector3.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

using namespace std;
using namespace NCL;
using namespace CSC8503;
using namespace std;

class LevelBuilder
{
public:
	LevelBuilder();
	~LevelBuilder();

	void BuildLevel();
protected:

	void InitialiseAssets();

	// Functions to add objects to the game world
	GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, GameWorld* world);


	// Meshes
	MeshGeometry* cubeMesh = nullptr;


	// Textures
	TextureBase* basicTex = nullptr;


	// Shaders
	ShaderBase* basicShader = nullptr;
};