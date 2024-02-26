#pragma once
#include "GameObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class SwingingObject : public Component
	{
	public:

		SwingingObject(GameObject* go) { gameObject = go; }
		void Update(float dt)override;

		void Start()override {
			startingPos = gameObject->GetTransform().GetPosition();
		}

		void PhysicsUpdate(float dt)override {

		}

	protected:
		float timer = 0;
		float const radius = 10;

		Vector3 startingPos;
	};
}

