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
    std::string     meshName = "";
    Vector3         dimensions = Vector3(0,0,0);
    Quaternion      rotation;
    Vector3         position = Vector3(0,0,0);
    float           inverseMass = 0;
    std::string     physicsType = "";
    Vector3         colliderExtents = Vector3(0,0,0);
    float           colliderRadius = 0;
    bool            shouldNetwork = false;

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

struct SwingingPrimitive : public PrimitiveGameObject {
public:
    float timePeriod;
    float cooldown;
    float waitDelay;
    float radius;
    bool changeAxis;
    bool changeDirection;
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

    std::string GetLevelName(int id) { return levelIDToLevelNameMap[id]; }
    void LoadLevelNameMap();

    [[nodiscard]] Vector3 GetDeathBoxPosition() const { return deathBoxPosition; } //TODO: rename this to deathPlane

    [[nodiscard]] std::vector<PrimitiveGameObject*> GetPrimitiveList() const { return primGOList; }
    [[nodiscard]] std::vector<OscillatorPrimitive*> GetOscillatorPList() const { return oscillatorPrimitives; }
    [[nodiscard]] std::vector<OscillatorPrimitive*> GetHarmfulOscillatorPList() const { return harmfulOscillatorPrimitives; }
    [[nodiscard]] std::vector<SwingingPrimitive*> GetSwingingPList() const { return swingingPrimitives; }
    [[nodiscard]] std::vector<SpringPrimitive*> GetSpringPList() const { return springPrimitives; }
    [[nodiscard]] std::vector<PointLightInfo> GetPointLights() const { return pointLights; }
    [[nodiscard]] Vector3 GetMedalTimes() const { return medalTimes; }

    [[nodiscard]] std::vector<GroundCubePrimitive*> GetGroundCubes() const { return groundCubes; }
    [[nodiscard]] std::vector<Vector3> GetCheckPointPositions() const { return checkPointPositions; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetSpeedupBlockPrimitiveList() const { return speedupBlockPrimitives; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetBridgePrimitiveList() const { return bridgePrimitives; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetTrapBlockPrimitiveList() const { return trapBlockPrimitives; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetRayEnemyPrimitiveList() const { return rayEnemyPrimitives; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetRayTriggerPrimitiveList() const { return rayenemytriggerPrimitives; }
    [[nodiscard]] std::vector<PrimitiveGameObject*> GetBridgeTriggerPrimitiveList() const { return bridgetriggerPrimitives; }


    int GetLevelSize() const { return primGOList.size();}
	bool HasReadLevel(const std::string& source);

    int GetNumberOfLevels() { return levelIDToLevelNameMap.size(); }

    void Clear();

protected:
	Vector3 startPosition;
	Vector3 endPosition;
    Vector3 deathBoxPosition;
    std::vector<Vector3> checkPointPositions;
    std::vector<PrimitiveGameObject*> primGOList;
    std::vector<OscillatorPrimitive*> oscillatorPrimitives;
    std::vector<OscillatorPrimitive*> harmfulOscillatorPrimitives;
    std::vector<SwingingPrimitive*> swingingPrimitives;
    std::vector<SpringPrimitive*> springPrimitives;
    std::vector<PointLightInfo> pointLights;

    std::vector<PrimitiveGameObject*> speedupBlockPrimitives;
    std::vector<PrimitiveGameObject*> bridgePrimitives;
    std::vector<PrimitiveGameObject*> trapBlockPrimitives;
    std::vector<PrimitiveGameObject*> rayEnemyPrimitives;
    std::vector<PrimitiveGameObject*> rayenemytriggerPrimitives;
    std::vector<PrimitiveGameObject*> bridgetriggerPrimitives;

    Vector3 medalTimes; //plat, gold, then silver

    std::vector<GroundCubePrimitive*> groundCubes;

    std::unordered_map<int, std::string> levelIDToLevelNameMap;


};