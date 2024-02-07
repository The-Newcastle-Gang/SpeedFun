#pragma once

#include "Vector3.h"
#include "LevelReader.h"
#include "Replicated.h"

using namespace NCL;
using namespace CSC8503;

class LevelBuilder
{
public:
	LevelBuilder();
	~LevelBuilder();

	void BuildLevel(GameWorld* w);
protected:

};