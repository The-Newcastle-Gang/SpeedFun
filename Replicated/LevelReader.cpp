///<summary>
/// Reads from levelSource and populates on variables
///</summary>

#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;



bool LevelReader::HasReadLevel(const std::string &levelSource) {
    std::cout << "TEST";

    std::ifstream jFileStream(Assets::LEVELDIR + levelSource);

	if (!jFileStream) {
		return false;
	}

	json jData = json::parse(jFileStream);

	startPosition = Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"] * -1);
	endPosition = Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"] * -1);


	for (auto& item : jData["primitiveGameObject"].items()) {
        auto tempGOPrimitive = new PrimitiveGameObject();
        tempGOPrimitive->meshName = item.value()["mesh"];

		auto& curPosRef     = item.value()["position"];
		auto& curDimRef     = item.value()["dimensions"];
        auto& curCollExt    = item.value()["colliderExtents"];
        auto& curRot        = item.value()["rotation"];


        tempGOPrimitive->dimensions         = Vector3(curDimRef["x"], curDimRef["y"],( curDimRef["z"] * -1));
        tempGOPrimitive->position           = Vector3(curPosRef["x"], curPosRef["y"],( curPosRef["z"] * -1));
        tempGOPrimitive->colliderExtents    = Vector3(curCollExt["x"], curCollExt["y"], curCollExt["z"]);
        tempGOPrimitive->rotation           = Quaternion((float)curRot["x"], (float)curRot["y"],(float)curRot["z"],(float)curRot["w"]);
        tempGOPrimitive->shouldNetwork      = item.value()["shouldNetwork"];
        tempGOPrimitive->colliderRadius     = item.value()["colliderRadius"];
        tempGOPrimitive->inverseMass        = item.value()["inverseMass"];
        tempGOPrimitive->physicsType        = item.value()["physicType"];


        primGOList.emplace_back(tempGOPrimitive);
//        delete tempGOPrimitive;
	}

    return true;
}
