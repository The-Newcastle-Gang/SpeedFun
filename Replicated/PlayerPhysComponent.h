#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"
#include "Debug.h"
#include "InputListener.h"
#include "GameWorld.h"


namespace NCL::CSC8503 {
	class GameObject;
	class PlayerPhysComponent : public Component
	{
	public:

		PlayerPhysComponent(GameObject* go, GameWorld* world);
		void PhysicsUpdate(float dt)override;
        float getRunVelocity(){ return runForce; }

    protected:
        void FastFalling(PhysicsObject* physGameObj);
        void ClampPlayerVelocity(PhysicsObject* physGameObj);
        void MinimizeSlide(PhysicsObject* physGameObj);
//        void 🦵💨❌();



        GameWorld* world;
        float runForce;
        float maxVelocity;
        float drag;
        float airDrag;
        float fastFallingMultiplier;
	};

}

