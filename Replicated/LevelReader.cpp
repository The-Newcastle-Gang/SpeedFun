#include "LevelReader.h"

using namespace NCL::CSC8503;

using json = nlohmann::json;

void LevelReader::ReadLevel() {
	std::ifstream jFileStream(Assets::LEVELDIR +"level.json");
	json jData = json::parse(jFileStream);


	//cout << jData["StartPoint"]["x"] + 1; 
	StartPosition = new Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"]);
	EndPosition = new Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"]);
	cout << "SP: " << *StartPosition <<endl;
	cout << "EP: " << *EndPosition << endl;
	GroundCubePrimitive* gp = new GroundCubePrimitive();

	for (const auto& item : jData["GroundBlocks"].items()) {
		for (const auto& x : item.value().items()) {
			cout << x << "\n";
		}
		//cout << item << "\n";
	}

}