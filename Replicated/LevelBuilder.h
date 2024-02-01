#pragma once
#include "GameObject.h"
#include "Vector3.h"

using namespace std;
using namespace NCL;
using namespace CSC8503;
using namespace std;

class LevelBuilder
{
public:
	void BuildLevel();
protected:
	
	GameObject* test;
	bool levelBuilt = false;
};