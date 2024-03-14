#include "TestBridge.h"

using namespace NCL;
using namespace CSC8503;

void TestBridge::Update(float dt) {
	BridgeFall(dt);
}

void TestBridge::BridgeFall(float dt) {
	if (bridgeFall) {
		currentOrientation = gameObject->GetTransform().GetOrientation();
		fallOrientation = currentOrientation * Quaternion::EulerAnglesToQuaternion(0.0, 0.0, 4 * sin(dt * PI));
		gameObject->GetTransform().SetOrientation(fallOrientation);
	}
}

void TestBridge::TriggerOn() {
	std::cout << "Bridge fall!\n";
	bridgeFall = !bridgeFall;
}

