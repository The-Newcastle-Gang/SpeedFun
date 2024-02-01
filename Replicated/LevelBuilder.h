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

};