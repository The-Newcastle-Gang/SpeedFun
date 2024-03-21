#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "TestBridge.h"

using namespace NCL;
using namespace CSC8503;

class BridgeTrigger :public Component {
public:
	BridgeTrigger(GameObject* go);
	void SetBridgeTrigger(TestBridge* it) { isTrigger = it; }
	void OnCollisionEnter(GameObject* otherObject) override;


protected:
	TestBridge* isTrigger;

};
