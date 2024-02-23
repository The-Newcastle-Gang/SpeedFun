#include "Spring.h"

Spring::Spring(GameObject* g, Vector3 bI, bool c, float ctfl, Vector3 cf) {
	gameObject = g;
	bounceForce = bI;
	continuous = c;
	continuousForceTimerLimit = ctfl;
	continuousForce = cf;
	
}

void Spring::Update(float dt) {
	
	std::vector<const GameObject*> tempList;
	for (auto obj : pushableObject) {
		if (continuous) {
			obj.first->GetPhysicsObject()->AddForce(continuousForce);
		}
		pushableObject[obj.first] -= dt;
		//std::cout << pushableObject[obj.first] << "\n";
		if (pushableObject[obj.first] <= 0.0f) tempList.push_back(obj.first);
	}

	for (auto obj : tempList) {
		auto it = pushableObject.find(obj);
		pushableObject.erase(it);
	}
}

void Spring::OnCollisionEnter(const GameObject* otherObject) {
	if (otherObject->GetIsPlayerBool() && pushableObject.find(otherObject) == pushableObject.end()) {
		pushableObject.emplace(otherObject, continuousForceTimerLimit);
		otherObject->GetPhysicsObject()->AddForce(bounceForce);
	}

}
