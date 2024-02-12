#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class PlayerPhysComponent : public Component
	{
	public:

		PlayerPhysComponent(GameObject* go) { gameObject = go; }
		void PhysicsUpdate(float dt)override;
	};
}

