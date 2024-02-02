///<summary>
/// Reads from levelSource and populates on variables
///</summary>

#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

bool LevelReader::ReadLevel(const std::string& levelSource) {

	std::ifstream jFileStream(Assets::LEVELDIR + levelSource);

	if (!jFileStream) {
		return false;
	}

	json jData = json::parse(jFileStream);

	startPosition = Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"] * -1);
	endPosition = Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"] * -1);

	for (auto& item : jData["GroundBlocks"].items()) {
		auto tempGroundPrimitive = new GroundCubePrimitive();
		auto& curPosRef = item.value()["position"];
		auto& curDimRef = item.value()["dimensions"];


		tempGroundPrimitive->pos = Vector3(curPosRef["x"], curPosRef["y"], curPosRef["z"] * -1);
		tempGroundPrimitive->dims = Vector3(curDimRef["x"], curDimRef["y"], curDimRef["z"]);
		tempGroundPrimitive->dims *= 0.5f;
		groundCubes.push_back(tempGroundPrimitive);

	}

	return true;
}