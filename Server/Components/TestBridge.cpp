#include "TestBridge.h"

using namespace NCL;
using namespace CSC8503;

void TestBridge::Update(float dt) {
	if (bridgeFall) {
		BridgeFall(dt);
	}
}

void TestBridge::BridgeFall(float dt) {
	fallTime += dt;
	if (fallTime <= 7.168) {
		currentOrientation = gameObject->GetTransform().GetOrientation();
		fallOrientation = currentOrientation * Quaternion::EulerAnglesToQuaternion(0.0, 0.0, 4 * sin(dt * PI));
		gameObject->GetTransform().SetOrientation(fallOrientation);
	}
	else {
		bridgeFall = false;
		fallTime = 0;
	}
}

void TestBridge::TriggerOn() {
	std::cout << "Bridge fall!\n";
	bridgeFall = true;
}

