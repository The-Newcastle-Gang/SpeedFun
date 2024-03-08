#include "TestBridge.h"

using namespace NCL;
using namespace CSC8503;

void TestBridge::Update(float dt) {
	if (bridgeFall) {
		BridgeFall(dt);
	}
}

void TestBridge::OnCollisionEnter(GameObject* otherObject) {
	if (otherObject->GetTag() == Tag::PLAYER) {
		bridgeFall = true;
	}
}

void TestBridge::BridgeFall(float dt) {
	fallTime += dt;
	if (fallTime <= 1) {
		currentOrientation = gameObject->GetTransform().GetOrientation();
		newOrientation = currentOrientation * Quaternion::EulerAnglesToQuaternion(90.0*(fallTime/11000), 0.0, 0.0);
		gameObject->GetTransform().SetOrientation(newOrientation);
	}
	else {
		bridgeFall = false;
	}
	
}

