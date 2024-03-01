#include "TrapObject.h"

using namespace NCL;
using namespace CSC8503;

void TestTrapBlock::Update(float dt) {
	if (blockFall) {
		TrapBlockFall(dt);
	}
}

void TestTrapBlock::OnCollisionEnter(GameObject* otherObject) {
	if (pushableObject.find(otherObject) == pushableObject.end()) {
		blockFall = true;
	}
}

void TestTrapBlock::TrapBlockFall(float dt) {
	warningTime += dt;
	if (warningTime >= 5) {
		gameObject->GetTransform().SetPosition(Vector3(-107, -5 - 10 * warningTime, -15));
		warningTime = 0.0;
	}
}