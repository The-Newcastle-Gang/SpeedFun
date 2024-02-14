#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"
#include "Debug.h"
#include "InputListener.h"
#include "GameWorld.h"
#include "Window.h"
#include "GrappleComponent.h"


namespace NCL::CSC8503 {
	class GameObject;
	class PlayerPhysComponent : public Component
	{
	public:

		PlayerPhysComponent(GameObject* go, GameWorld* world);
        void PhysicsUpdate(float dt)override;

        void ProcessMovementInput(Vector3 fwdAxis, Vector3 rightAxis, Vector2 playerInput);
        void ProcessJumpInput(float jumpKeyPresed);



        float getRunVelocity()      { return runForce; }
        bool  checkIsGrounded()     { return isGrounded;}
        float getAirMultiplier()    { return airMultiplier;}

    protected:
        void FastFalling(PhysicsObject* physGameObj);
        void ClampPlayerVelocity(PhysicsObject* physGameObj);
        void MinimizeSlide(PhysicsObject* physGameObj);

        void GroundCheck(PhysicsObject* physGameObj, Vector3 position);

//        void 🦵💨❌();



        GameWorld* world;
        float runForce;
        float maxVelocity;
        float drag;
        float airDrag;
        float groundOffset;
        float jumpForce;
        float airMultiplier;
        float fastFallingMultiplier;
        bool isGrounded;
        bool isGrappling;
	};

}

