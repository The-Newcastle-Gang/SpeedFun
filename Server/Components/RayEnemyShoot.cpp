#include "RayEnemyShoot.h"

using namespace NCL;
using namespace CSC8503;

void RayEnemyShoot::Update(float dt) {
	if (goShoot) {
		EnemyShoot(dt);
	}
}

void RayEnemyShoot::EnemyShoot(float dt) {
	warningTime += dt;
	if (warningTime >= 5) {
		//std::cout << "player be shoot" << std::endl;
		warningTime = 0.0;
	}
}

void RayEnemyShoot::OnCollisionEnter(GameObject* otherObject) {
	if (otherObject->GetTag() == Tag::PLAYER) {
		followComponent->OnPlayerEnter(otherObject);
		//std::cout << "Enter the warning area!!!" << std::endl;
		goShoot = true;
	}
}

void RayEnemyShoot::OnCollisionEnd(GameObject* otherObject) {
	if (otherObject->GetTag() == Tag::PLAYER) {
		//std::cout << "Leave the warning area" << std::endl;
		goShoot = false;
	}
}