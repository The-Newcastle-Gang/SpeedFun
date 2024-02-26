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
		void Update(float dt)override {
			timer += dt;
			gameObject->GetTransform().SetPosition({ 1, sin(timer) * movementModifier, cos(timer) * movementModifier });
		}

		void Start()override {
		}

		void PhysicsUpdate(float dt)override {

		}

	protected:
		float timer = 0;
		float const movementModifier = 5;
	};
}

