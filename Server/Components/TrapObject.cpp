#include "TrapObject.h"

using namespace NCL;
using namespace CSC8503;

void TrapBlock::Update(float dt) {
	if (blockFall) {
		TrapBlockFall(dt);
	}
}

void TrapBlock::OnCollisionEnter(GameObject* otherObject) {
	if (otherObject->GetTag() == Tag::PLAYER) {
		blockFall = true;
	}
	
}

void TrapBlock::TrapBlockFall(float dt) {
	warningTime += dt;
	if (warningTime >= 5) {
		gameObject->GetTransform().SetPosition(Vector3(-107, -5 - 5 * warningTime, -15));
		warningTime = 0.0;
	}
}