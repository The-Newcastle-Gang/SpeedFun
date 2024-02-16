//
// Created by idhant on 12-02-2024.
//
#include "PlayerPhysComponent.h"
using namespace NCL::CSC8503;

PlayerPhysComponent::PlayerPhysComponent(GameObject *go, GameWorld* pWorld) {

    world                       = pWorld;
    gameObject                  = go;
    gravity                     = -9.8f;


    runForce                    = 50.0f;
    groundOffset                = 0.1f;
    jumpForce                   = 3.0f;
    airMultiplier               = 1.0f;


    isGrounded                  = false;
    isGrappling                 = false;
    isDashing                   = false;

    stateManager = std::make_unique<StateMachine>();
    InitStateMachine();
}

void PlayerPhysComponent::InitStateMachine() {
    auto playerPhysObj = gameObject->GetPhysicsObject();
    auto grounded = new GroundedState(playerPhysObj);
    auto jumping = new JumpingState(playerPhysObj);
    auto grappling = new GrapplingState(playerPhysObj);

    stateManager->AddState(grounded);
    stateManager->AddState(jumping);
    stateManager->AddState(grappling);


    //TODO:: REPLACE ALL THE IS GRAPPLING WITH HAS GRAPPLE FINISHED SOMEHOW

    stateManager->AddTransition(new StateTransition(grounded, jumping, [=, this]()-> bool{
        return !isGrounded;
    }));

    stateManager->AddTransition(new StateTransition(grounded, grappling, [=, this]()-> bool{
        return isGrappling && isGrounded;
    }));

    stateManager->AddTransition(new StateTransition(jumping, grounded, [=, this]()->bool{
        return isGrounded;
    }));

    stateManager->AddTransition(new StateTransition(jumping, grappling, [=, this]()->bool{
        return isGrappling;
    }));

    stateManager->AddTransition(new StateTransition(grappling, grounded, [=, this]()-> bool{
        return grappling->HasGrappleCompleted();
    }));

//    stateManager->AddTransition(new StateTransition(grappling, jumping, [=, this]()-> bool{
//        //?
//    }));


}

void PlayerPhysComponent::ProcessMovementInput(Vector3 fwdAxis, Vector3 rightAxis, Vector2 playerInput) {

    auto pGO = gameObject->GetPhysicsObject();

    pGO->AddForce(fwdAxis  *playerInput.y * runForce * airMultiplier);
    pGO->AddForce(rightAxis *playerInput.x * runForce * airMultiplier);

}

void PlayerPhysComponent::Update(float dt) {


    auto pGO = gameObject->GetPhysicsObject();
    GroundCheck(pGO, gameObject->GetTransform().GetPosition());
}

void PlayerPhysComponent::PhysicsUpdate(float dt) {

    if(!isGrounded){
        airMultiplier = 0.3f;
    } else{
        airMultiplier = 1.0f;
    }

    auto physGameObj = gameObject->GetPhysicsObject();
    if(!isGrounded){
        physGameObj->AddForce(Vector3(0,gravity * 0.5f,0));
    }

    stateManager->Update(dt);
}

//multiply velocity instead of force since forces can be dangerous
void PlayerPhysComponent::FastFalling(PhysicsObject* physGameObj) {


}


void PlayerPhysComponent::GroundCheck(PhysicsObject *physGameObj, Vector3 position) {

    auto collVol = (CapsuleVolume*)gameObject->GetBoundingVolume();
    Vector3 capBottom =  position - Vector3(0, collVol->GetHalfHeight(),0);
    capBottom += Vector3(0, groundOffset, 0);

    Ray ray = Ray(capBottom, Vector3(0,-1,0));
    RayCollision closestCollision ;

    float groundEpsilon = 0.01;

    if(world->Raycast(ray, closestCollision, true, gameObject)){
        Vector3 disp = closestCollision.collidedAt - capBottom;

        if(disp.Length() < groundOffset + groundEpsilon){
           isGrounded = true;
        }
        else {
            isGrounded = false;
        }
    }
    else {
        isGrounded = false;
    }
}

void PlayerPhysComponent::Jump() {
    if(isGrounded)
    {
        gameObject->GetPhysicsObject()->ApplyLinearImpulse(Vector3{0,1,0} * jumpForce);
    }
}

