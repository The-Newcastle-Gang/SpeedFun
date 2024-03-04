///<summary>
/// Reads from levelSource and populates on variables
///</summary>

#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

LevelReader::LevelReader(){

}

LevelReader::~LevelReader(){

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


    return true;
}
