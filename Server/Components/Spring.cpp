#include "Spring.h"

Spring::Spring(GameObject* g, Vector3 bI,  float aT, bool c, Vector3 cf) {
	gameObject = g;
	bounceForce = bI;
	springActiveTime = aT;
	continuous = c;
	continuousForce = cf;
	
}

void Spring::Update(float dt) {
	
	std::vector<const GameObject*> tempList;
	for (auto& obj : pushableObject) {
		if (continuous) {
			obj.first->GetPhysicsObject()->AddForce(continuousForce);
		}
		pushableObject[obj.first] -= dt;
		if (pushableObject[obj.first] <= 0.0f) tempList.push_back(obj.first);
	}

	for (auto& obj : tempList) {
		auto it = pushableObject.find(obj);
		pushableObject.erase(it);
	}
}

void Spring::OnCollisionEnter(GameObject* otherObject) {
	if (pushableObject.find(otherObject) == pushableObject.end()) {
		pushableObject.emplace(otherObject, springActiveTime);
		otherObject->GetPhysicsObject()->AddForce(bounceForce);
	}
}
