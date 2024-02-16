#pragma once
#include "GameObject.h"
#include "Component.h"
#include "PhysicsObject.h"
#include "Debug.h"
#include "InputListener.h"
#include "GameWorld.h"
#include "GrappleComponent.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "../Server/PlayerStates/GroundedState.h"
#include "../Server/PlayerStates/JumpingState.h"
#include "../Server/PlayerStates/GrapplingState.h"


namespace NCL::CSC8503 {
	class GameObject;
	class PlayerPhysComponent : public Component
	{
	public:

		PlayerPhysComponent(GameObject* go, GameWorld* world);
        void PhysicsUpdate(float dt) override;
        void Update(float dt) override;

        void ProcessMovementInput(Vector3 fwdAxis, Vector3 rightAxis, Vector2 playerInput);

        float GetRunVelocity()   const  { return runForce; }
        bool  CheckIsGrounded()  const  { return isGrounded;}
        float GetAirMultiplier() const  { return airMultiplier;}
        void  SetGrappling(bool x)      { isGrappling = x; }

        void SetIsDashing(bool b)       {isDashing = b;}
        bool GetIsDashing() const       { return isDashing; }

        void Jump();

        void InitStateMachine();

    protected:
        void FastFalling(PhysicsObject* physGameObj);
        void GroundCheck(PhysicsObject* physGameObj, Vector3 position);


        GameWorld* world;
        float gravity               = 0.0f;
        float runForce              = 0.0f;
        float airDrag               = 0.0f;
        float groundOffset          = 0.0f;
        float jumpForce             = 0.0f;
        float airMultiplier         = 0.0f;
        float fastFallingMultiplier = 0.0f;
        bool isGrounded             = false;
        bool isGrappling            = false;
        bool isDashing              = false;


        std::unique_ptr<StateMachine> stateManager;

	};

}

