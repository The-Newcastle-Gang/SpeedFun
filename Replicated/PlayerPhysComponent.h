#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"
#include "Debug.h"
#include "InputListener.h"
#include "GameWorld.h"
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


        float getRunVelocity()   const  { return runForce; }
        bool  checkIsGrounded()  const  { return isGrounded;}
        float getAirMultiplier() const  { return airMultiplier;}
        void setGrappling(bool x)       { isGrappling = x; }

        void SetIsDashing(bool b) {isDashing = b;}

    protected:
        void FastFalling(PhysicsObject* physGameObj);
        void ClampPlayerVelocity(PhysicsObject* physGameObj);
        void MinimizeSlide(PhysicsObject* physGameObj);

        void GroundCheck(PhysicsObject* physGameObj, Vector3 position);


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
        bool isDashing;
	};

}

