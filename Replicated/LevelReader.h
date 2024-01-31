#pragma once
#include <json.hpp>
#include <iostream>
#include <fstream> 
#include <Vector3.h>
#include <GameObject.h>
#include <Assets.h>
#include <json.hpp>
#include <GameObject.h>
#include <set>

using namespace std;
using namespace NCL;
using namespace CSC8503;
using namespace std;


class GroundCubePrimitive {
public:
	Vector3* pos;
	Vector3* dims;
};

class LevelReader {
public:
	void ReadLevel();
protected:
	Vector3* StartPosition;
	Vector3* EndPosition;
	vector<GroundCubePrimitive> GroundCubes;

};