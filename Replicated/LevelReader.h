#pragma once
#include <json.hpp>
#include <iostream>
#include <fstream> 
#include <Vector3.h>
#include <GameObject.h>
#include "GameWorld.h"

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

struct OscillatorPrimitive : public PrimitiveGameObject {
public:
    float           timePeriod;
    float           distance;
    Vector3         direction;
    float           cooldown;
    float           waitDelay;
};

struct SpringPrimitive : public PrimitiveGameObject {
public:
    float           force;
    Vector3         direction;
    float           activeTime;
    bool           isContinuous;
    float           continuousForce;
};

class LevelReader {
public:

    LevelReader();
    ~LevelReader();

	Vector3 GetStartPosition() const { return startPosition; }
	Vector3 GetEndPosition() const { return endPosition; }
    [[nodiscard]] Vector3 GetDeathBoxPosition() const { return deathBoxPosition; } //TODO: rename this to deathPlane

    [[nodiscard]] std::vector<PrimitiveGameObject*> GetPrimitiveList() const { return primGOList; }
    [[nodiscard]] std::vector<OscillatorPrimitive*> GetOscillatorPList() const { return oscillatorPrimitives; }
    [[nodiscard]] std::vector<OscillatorPrimitive*> GetHarmfulOscillatorPList() const { return harmfulOscillatorPrimitives; }
    [[nodiscard]] std::vector<SpringPrimitive*> GetSpringPList() const { return springPrimitives; }
    [[nodiscard]] std::vector<PointLightInfo> GetPointLights() const { return pointLights; }

    [[nodiscard]] std::vector<GroundCubePrimitive*> GetGroundCubes() const { return groundCubes; }
    [[nodiscard]] std::vector<Vector3> GetCheckPointPositions() const { return checkPointPositions; }

    int GetLevelSize() const { return primGOList.size();}
	bool HasReadLevel(const std::string& source);

protected:
	Vector3 startPosition;
	Vector3 endPosition;
    Vector3 deathBoxPosition;
    std::vector<Vector3> checkPointPositions;
    std::vector<PrimitiveGameObject*> primGOList;
    std::vector<OscillatorPrimitive*> oscillatorPrimitives;
    std::vector<OscillatorPrimitive*> harmfulOscillatorPrimitives;
    std::vector<SpringPrimitive*> springPrimitives;
    std::vector<PointLightInfo> pointLights;

    std::vector<GroundCubePrimitive*> groundCubes;

};