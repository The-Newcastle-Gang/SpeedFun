#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "Maths.h"

using namespace NCL;
using namespace CSC8503;


class TestBridge : public Component {
public:
	TestBridge(GameObject* go) { gameObject = go; }
	void Update(float dt) override;
	void OnCollisionEnter(GameObject* otherObject) override;
	void BridgeFall(float dt);

protected:
	bool bridgeFall = false;
	double fallTime = 0.0;
	Quaternion currentOrientation;
	Quaternion newOrientation;
};