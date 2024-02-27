#pragma once
#include "GameObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class TestTrapBlock : public Component
	{
	public:

		TestTrapBlock(GameObject* go) { gameObject = go; }

		void Update(float dt)override;

		void TrapBlockFall(float dt);
		
		bool hasCollision(const GameObject* otherObject);

	protected:
		double warningTime = 0.0;

	};
}



