#pragma once
#include "GameObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class TestComponent : public Component
	{
	public:

		TestComponent(GameObject* go) { gameObject = go; }
		void Update(float dt)override {
			std::cout << "UPDATE TEST" << "\n";
		}

		void Start()override {
			std::cout << "START TEST" << "\n";
		}

		void PhysicsUpdate(float dt)override {
			std::cout << "PHYS UPDATE TEST" << "\n";
		}
	};
}

