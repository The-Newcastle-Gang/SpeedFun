#pragma once
#include <json.hpp>
#include <iostream>
#include <fstream> 
#include <Vector3.h>
#include <GameObject.h>
#include <Assets.h>
#include <json.hpp>
#include <set>

using namespace std;
using namespace NCL;
using namespace CSC8503;
using namespace std;


class GroundCubePrimitive {
public:
	Vector3 pos;
	Vector3 dims;
};

class LevelReader {
public:

	Vector3 GetStartPosition() { return startPosition; }
	Vector3 GetEndPosition() { return endPosition; }
	std::vector<GroundCubePrimitive*> GetGroundCubes() { return groundCubes; }

	void ReadLevel(std::string src);
protected:
	Vector3 startPosition;
	Vector3 endPosition;
	std::vector<GroundCubePrimitive*> groundCubes;

};