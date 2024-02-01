///<summary>
/// Reads from leveljson and populates on variables
///</summary>

#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

bool LevelReader::ReadLevel(std::string source) {

	std::ifstream jFileStream(Assets::LEVELDIR + source);

	if (!jFileStream) {
		return false;
	}

	json jData = json::parse(jFileStream);

	startPosition = Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"] * -1);
	endPosition = Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"] * -1);

	for (auto& item : jData["GroundBlocks"].items()) {
		GroundCubePrimitive* tempgroundprim = new GroundCubePrimitive();
		auto& curposref = item.value()["position"];
		auto& curdimref = item.value()["dimensions"];


		tempgroundprim->pos = Vector3(curposref["x"], curposref["y"], curposref["z"] * -1);
		tempgroundprim->dims = Vector3(curdimref["x"], curdimref["y"], curdimref["z"]);
		tempgroundprim->dims *= 0.5f;
		groundCubes.push_back(tempgroundprim);

	}

	return true;
}