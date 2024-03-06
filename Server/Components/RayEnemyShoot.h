#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include <functional>

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;

	class RayEnemyShoot :public Component {
	public:
		RayEnemyShoot(GameObject* go) { gameObject = go; }

		void Update(float dt) override;
		void OnCollisionEnter(GameObject* otherObject) override;
		void OnCollisionEnd(GameObject* otherObject)override;
		void EnemyShoot(float dt);

	protected:
		bool goShoot = false;
		double warningTime = 0.0;
	};
}