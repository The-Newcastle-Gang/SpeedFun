#pragma once
#include <json.hpp>
#include <iostream>
#include <fstream> 
#include <Vector3.h>
#include <GameObject.h>
#include <Assets.h>
#include <json.hpp>
#include <set>

using namespace NCL;
using namespace CSC8503;

struct PrimitiveGameObject {
public:
    std::string     meshName;
    Vector3         dimensions;
    Quaternion      rotation;
    Vector3         position;
    float           inverseMass;
    std::string     physicsType;
    Vector3         colliderExtents;
    float           colliderRadius;
    bool            shouldNetwork;

};


struct GroundCubePrimitive {
public:
	Vector3 pos;
	Vector3 dims;
};

class LevelReader {
public:

	Vector3 GetStartPosition() { return startPosition; }
	Vector3 GetEndPosition() { return endPosition; }
    std::vector<PrimitiveGameObject*> GetPrimitiveList(){ return primGOList; }


    std::vector<GroundCubePrimitive*> GetGroundCubes() { return groundCubes; }

	bool ReadLevel(const std::string& source);
protected:
	Vector3 startPosition;
	Vector3 endPosition;
    std::vector<PrimitiveGameObject*> primGOList;

    std::vector<GroundCubePrimitive*> groundCubes;

};