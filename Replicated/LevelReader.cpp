///<summary>
/// Reads from levelSource and populates on variables
///</summary>

#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

LevelReader::LevelReader(){
    LoadLevelNameMap();
}

LevelReader::~LevelReader(){

}

void LevelReader::Clear() { //we need to free all the memory so we dont leak
    for (auto& i : primGOList) {
        delete i;
        i = nullptr;
    }
    primGOList.clear();

    for (auto& i : oscillatorPrimitives) {
        delete i;
        i = nullptr;
    }
    oscillatorPrimitives.clear();

    for (auto& i : harmfulOscillatorPrimitives) {
        delete i;
        i = nullptr;
    }
    harmfulOscillatorPrimitives.clear();

    for (auto& i : springPrimitives) {
        delete i;
        i = nullptr;
    }
    springPrimitives.clear();

    for (auto& i : swingingPrimitives)
    {
        delete i;
        i = nullptr;
    }
    swingingPrimitives.clear();

    pointLights.clear();

    for (auto& i : groundCubes) {
        delete i;
        i = nullptr;
    }
    groundCubes.clear();
    
    for (auto& i : bridgePrimitives) {
        delete i;
        i = nullptr;
    }
    bridgePrimitives.clear();

    for (auto& i : speedupBlockPrimitives) {
        delete i;
        i = nullptr;
    }
    speedupBlockPrimitives.clear();

    for (auto& i : trapBlockPrimitives) {
        delete i;
        i = nullptr;
    }
    trapBlockPrimitives.clear();


    for (auto& i : rayEnemyPrimitives) {
        delete i;
        i = nullptr;
    }
    rayEnemyPrimitives.clear();


    for (auto& i : rayenemytriggerPrimitives) {
        delete i;
        i = nullptr;
    }
    rayenemytriggerPrimitives.clear();


    for (auto& i : bridgetriggerPrimitives) {
        delete i;
        i = nullptr;
    }
    bridgetriggerPrimitives.clear();

    medalTimes = Vector3(0, 0, 0);

    checkPointPositions.clear();
}

void LevelReader::LoadLevelNameMap() {
    int counter = 0;
    for (const auto& entry : std::filesystem::directory_iterator(Assets::LEVELDIR)) {
        std::string name {entry.path().filename().string()};

        //remove filename
        size_t last = name.find_last_of(".");
        if (last != std::string::npos) name = name.substr(0, last);

        levelIDToLevelNameMap[counter] = name;
        std::cout << name << std::endl;
        counter++;
    }
}

bool LevelReader::HasReadLevel(const std::string &levelSource) {

    std::ifstream jFileStream(Assets::LEVELDIR + levelSource);

	if (!jFileStream) {
		return false;
	}

	json jData = json::parse(jFileStream);

	startPosition = Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"] * -1);
	endPosition = Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"] * -1);
    deathBoxPosition = Vector3(jData["DeathPlane"]["x"],  jData["DeathPlane"]["y"],  jData["DeathPlane"]["z"] * -1);


    for(auto& item : jData["checkPoints"].items()){
        auto temp = Vector3(item.value()["x"], item.value()["y"], item.value()["z"] * -1);
        checkPointPositions.emplace_back(temp);
    }


	for (auto& item : jData["primitiveGameObject"].items()) {
        auto tempGOPrimitive = new PrimitiveGameObject();
        tempGOPrimitive->meshName = item.value()["mesh"];

		auto& curPosRef     = item.value()["position"];
		auto& curDimRef     = item.value()["dimensions"];
        auto& curCollExt    = item.value()["colliderExtents"];
        auto& curRot        = item.value()["rotation"];


        tempGOPrimitive->dimensions         = Vector3(curDimRef["x"], curDimRef["y"],( curDimRef["z"]));
        tempGOPrimitive->position           = Vector3(curPosRef["x"], curPosRef["y"],( curPosRef["z"] * -1));
        tempGOPrimitive->colliderExtents    = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        tempGOPrimitive->rotation           = Quaternion((float)curRot["x"], (float)curRot["y"],(float)curRot["z"],(float)curRot["w"]);
        tempGOPrimitive->shouldNetwork      = item.value()["shouldNetwork"];
        tempGOPrimitive->colliderRadius     = item.value()["colliderRadius"];
        tempGOPrimitive->inverseMass        = item.value()["inverseMass"];
        tempGOPrimitive->physicsType        = item.value()["physicType"];


        primGOList.emplace_back(tempGOPrimitive);
	}

    for(auto& item: jData["oscList"].items()){
        auto temp = new OscillatorPrimitive();

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->meshName = item.value()["mesh"];
        temp->timePeriod = (float)item.value()["timePeriod"];
        temp->distance = (float)item.value()["dist"];
        temp->cooldown = (float)item.value()["cooldown"];
        temp->waitDelay = (float)item.value()["waitDelay"];

        auto& dimref = item.value()["dimensions"];
        auto& posi = item.value()["position"];
        auto& dir = item.value()["direction"];
        
        temp->dimensions = Vector3(dimref["x"], dimref["y"], dimref["z"]);
        temp->position = Vector3(posi["x"], posi["y"], posi["z"] * -1);
        temp->direction = Vector3(dir["x"], dir["y"], dir["z"]* -1);

        temp->shouldNetwork = true;

        oscillatorPrimitives.emplace_back(temp);
    }

    for (auto& item : jData["harmOscList"].items()) {
        auto temp = new OscillatorPrimitive();

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->meshName = item.value()["mesh"];
        temp->timePeriod = (float)item.value()["timePeriod"];
        temp->distance = (float)item.value()["dist"];
        temp->cooldown = (float)item.value()["cooldown"];
        temp->waitDelay = (float)item.value()["waitDelay"];

        auto& dimref = item.value()["dimensions"];
        auto& posi = item.value()["position"];
        auto& dir = item.value()["direction"];

        temp->dimensions = Vector3(dimref["x"], dimref["y"], dimref["z"]);
        temp->position = Vector3(posi["x"], posi["y"], posi["z"] * -1);
        temp->direction = Vector3(dir["x"], dir["y"], dir["z"] * -1);

        temp->shouldNetwork = true;

        harmfulOscillatorPrimitives.emplace_back(temp);
    }

    for (auto& item : jData["springs"].items()) {
        auto temp = new SpringPrimitive();

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->meshName = item.value()["mesh"];
        temp->force = (float)item.value()["force"];
        temp->activeTime = (float)item.value()["activeTime"];
        temp->isContinuous = (float)item.value()["isContinuous"];
        temp->continuousForce = (float)item.value()["continuousForce"];

        //auto& dimref = item.value()["dimensions"];
        //auto& posi = item.value()["position"];
        auto& dir = item.value()["direction"];

        //temp->dimensions = Vector3(dimref["x"], dimref["y"], dimref["z"]);
        //temp->position = Vector3(posi["x"], posi["y"], posi["z"] * -1);
        temp->direction = Vector3(dir["x"], dir["y"], dir["z"] * -1);

        temp->shouldNetwork = false;

        springPrimitives.emplace_back(temp);
    }

    for (auto& item : jData["speedBlockList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];
        speedupBlockPrimitives.emplace_back(temp);
    }

    for (auto& item : jData["bridgeList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->shouldNetwork = true;

        bridgePrimitives.emplace_back(temp);
    }

    for (auto& item : jData["trapBlockList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->shouldNetwork = true;

        trapBlockPrimitives.emplace_back(temp);
    }


    for (auto& item : jData["rayenemyList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->shouldNetwork = true;

        rayEnemyPrimitives.emplace_back(temp);
    }


    for (auto& item : jData["rayenemytriList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->shouldNetwork = true;

        rayenemytriggerPrimitives.emplace_back(temp);
    }

    for (auto& item : jData["bridgetriList"].items()) {
        auto temp = new PrimitiveGameObject();
        temp->meshName = item.value()["mesh"];

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->shouldNetwork = true;

        bridgetriggerPrimitives.emplace_back(temp);
    }
    
    for (auto& item : jData["swingingList"].items()) {
        auto temp = new SwingingPrimitive();

        auto& curPosRef = item.value()["position"];
        auto& curDimRef = item.value()["dimensions"];
        auto& curCollExt = item.value()["colliderExtents"];
        auto& curRot = item.value()["rotation"];


        temp->dimensions = Vector3(curDimRef["x"], curDimRef["y"], (curDimRef["z"]));
        temp->position = Vector3(curPosRef["x"], curPosRef["y"], (curPosRef["z"] * -1));
        temp->colliderExtents = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        temp->rotation = Quaternion((float)curRot["x"], (float)curRot["y"], (float)curRot["z"], (float)curRot["w"]);
        temp->shouldNetwork = item.value()["shouldNetwork"];
        temp->colliderRadius = item.value()["colliderRadius"];
        temp->inverseMass = item.value()["inverseMass"];
        temp->physicsType = item.value()["physicType"];

        temp->meshName = item.value()["mesh"];

        temp->timePeriod = (float)item.value()["timePeriod"];
        temp->cooldown = (float)item.value()["cooldown"];
        temp->waitDelay = (float)item.value()["waitDelay"];
        temp->radius = (float)item.value()["radius"];
        temp->changeAxis = item.value()["changeAxis"];
        temp->changeDirection = item.value()["changeDirection"];

        auto& dimref = item.value()["dimensions"];
        auto& posi = item.value()["position"];
        auto& dir = item.value()["direction"];

        temp->dimensions = Vector3(dimref["x"], dimref["y"], dimref["z"]);
        temp->position = Vector3(posi["x"], posi["y"], posi["z"] * -1);

        temp->shouldNetwork = true;

        swingingPrimitives.emplace_back(temp);

    }

    for (auto& item : jData["pointLights"].items()) {
        PointLightInfo newLight;
        auto& colour = item.value()["colour"];
        auto& pos = item.value()["position"];
        auto& rad = item.value()["radius"];

        newLight.lightColour = Vector4((float)colour["x"], (float)colour["y"], (float)colour["z"], (float)colour["w"]);
        newLight.lightPosition = Vector3((float)pos["x"], (float)pos["y"], (float)pos["z"] * -1);
        newLight.lightRadius = (float)rad;

        pointLights.emplace_back(newLight);
    }

    auto& medalItem = jData["medalInfo"];


    try{
        auto& plat = medalItem["platinum"];
        auto& gold = medalItem["gold"];
        auto& silver = medalItem["silver"];
        medalTimes = Vector3(plat, gold, silver);
    }
    catch(...){
        medalTimes = Vector3(10, 20, 30);
    }

    return true;
}
