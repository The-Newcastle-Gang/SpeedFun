#include "TrapObject.h"

using namespace NCL;
using namespace CSC8503;

void TestTrapBlock::Update(float dt) {
	//bool BlockFall = hasCollision(gameObject);
	if (true) {
		TrapBlockFall(dt);
	}
}

bool TestTrapBlock::hasCollision(const GameObject* otherObject) {
	return true;
}

void TestTrapBlock::TrapBlockFall(float dt) {
	warningTime += dt;
	if (warningTime >= 10) {
		gameObject->GetTransform().SetPosition(Vector3(-110, -20, -10));
		warningTime = 0.0;
	}
}