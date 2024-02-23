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

			std::cout << "in the component!!!!!" << gameObject->GetName() << std::endl;
			gameObject->GetTransform().SetPosition({ 1, (float)(rand() % 6), 1 });
		}

		void Start()override {

		}

		void PhysicsUpdate(float dt)override {

		}
	};
}

