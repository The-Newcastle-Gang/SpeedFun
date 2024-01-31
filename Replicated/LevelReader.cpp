#include "LevelReader.h"
using namespace NCL::CSC8503;
using json = nlohmann::json;

void LevelReader::ReadLevel() {

	std::ifstream jFileStream(Assets::LEVELDIR +"level.json");
	json jData = json::parse(jFileStream);

	StartPosition = new Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"]);
	EndPosition = new Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"]);

	GroundCubePrimitive* tempgroundprim = new GroundCubePrimitive();

	for (auto& item : jData["GroundBlocks"].items()) {

		auto curposref = item.value()["position"];
		auto curdimref = item.value()["dimensions"];

		tempgroundprim->pos = new Vector3(curposref["x"], curposref["y"], curposref["z"]);
		tempgroundprim->dims = new Vector3(curdimref["x"], curdimref["y"], curdimref["z"]);
		GroundCubes.emplace_back(&tempgroundprim);

	}

	cout << GroundCubes.size();
	

}