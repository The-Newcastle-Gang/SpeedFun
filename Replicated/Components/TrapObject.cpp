#include "TrapObject.h"

using namespace NCL;
using namespace CSC8503;

void TestTrapBlock::Update(float dt) {
	//bool collisionResult = OnCollisionEnter(gameObject);
	if (blockFall) {
		TrapBlockFall(dt);
	}
}

//bool TestTrapBlock::hasCollision(const GameObject* otherObject) {
//	return true;
//}
bool TestTrapBlock::OnCollisionEnter(GameObject* otherObject) {
	if (pushableObject.find(otherObject) != pushableObject.end()) {
		std::cout << "cnm" << std::endl;
		return true;
	}
	else {
		return false;
	}
}

void TestTrapBlock::TrapBlockFall(float dt) {
	warningTime += dt;
	if (warningTime >= 5) {
		gameObject->GetTransform().SetPosition(Vector3(-107, -5 - 10 * warningTime, -15));
		//warningTime = 0.0;
	}
}