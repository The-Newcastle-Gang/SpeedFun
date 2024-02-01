#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

void LevelReader::ReadLevel() {

	std::ifstream jFileStream(Assets::LEVELDIR +"level.json");
	json jData = json::parse(jFileStream);

	startPosition = Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"]);
	endPosition = Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"]);

	for (auto& item : jData["GroundBlocks"].items()) {
		GroundCubePrimitive* tempgroundprim = new GroundCubePrimitive();
		auto& curposref = item.value()["position"];
		auto& curdimref = item.value()["dimensions"];


		tempgroundprim->pos = Vector3(curposref["x"], curposref["y"], curposref["z"]);
		tempgroundprim->dims = Vector3(curdimref["x"], curdimref["y"], curdimref["z"]);
		groundCubes.push_back(tempgroundprim);

	}

	cout << groundCubes.size();

}