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
			std::cout << "UPDATE ON THE COMPONENT!!!!!";
		}

		void Start()override {

		}

		void PhysicsUpdate(float dt)override {
			std::cout << "PHYS UPDATE TEST" << "\n";
		}
	};
}

