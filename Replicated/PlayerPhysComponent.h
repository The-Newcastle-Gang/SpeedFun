#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"
#include "Debug.h"
#include "InputListener.h"


namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class PlayerPhysComponent : public Component
	{
	public:

		PlayerPhysComponent(GameObject* go);
		void PhysicsUpdate(float dt)override;
        float getRunVelocity(){ return runForce; }

    protected:
        float runForce;
        float maxVelocity;
        float drag;
        float airDrag;
        float fastFallingMultiplier;
	};

}

