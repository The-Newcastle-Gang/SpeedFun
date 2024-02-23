#include "Spring.h"

Spring::Spring(GameObject* g, Vector3 bI) {
	gameObject = g;
	bounceForce = bI;
}

void Spring::Update(float dt) {

	cooldown -= dt;

}

void Spring::OnCollisionEnter(const GameObject* otherObject) {
	if (otherObject->GetIsPlayerBool() && cooldown <=0 ) {
		cooldown = cooldownLimit;
		
		otherObject->GetPhysicsObject()->AddForce(bounceForce);
	}

}
