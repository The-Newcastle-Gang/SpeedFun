#include "LevelReader.h"

using namespace NCL::CSC8503;

using json = nlohmann::json;

void LevelReader::ReadLevel() {
	std::ifstream jFileStream(Assets::LEVELDIR +"level.json");
	json jData = json::parse(jFileStream);


	//cout << jData["StartPoint"]["x"] + 1; 
	StartPosition = new Vector3(jData["StartPoint"]["x"], jData["StartPoint"]["y"], jData["StartPoint"]["z"]);
	EndPosition = new Vector3(jData["EndPoint"]["x"], jData["EndPoint"]["y"], jData["EndPoint"]["z"]);

	for (auto& x : jData.items()) {
		cout << "key" << x.key() << "value" << x.value() << endl;
	}


	/*cout << *StartPosition;
	cout << "hehehhehe";*/
}