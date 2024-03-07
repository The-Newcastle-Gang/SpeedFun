#include "SpeedUpBlock.h"

using namespace NCL;
using namespace CSC8503;

void TestSpeedUpBlock::Update(float dt) {
	//timer += dt;
}


void TestSpeedUpBlock::OnCollisionEnter(GameObject* other) {

	if (other->GetTag() == PLAYER) {
		//timer = 0.0;
		const float Speed1 = 45000.0f;
		const float Speed2 = 1000.0f;
		other->GetPhysicsObject()->AddForce(Vector3(-1, 0, 0) * Speed1);
		other->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * Speed2);
	}
}

