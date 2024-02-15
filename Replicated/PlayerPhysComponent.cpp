//
// Created by idhant on 12-02-2024.
//
#include "PlayerPhysComponent.h"
using namespace NCL::CSC8503;

PlayerPhysComponent::PlayerPhysComponent(GameObject *go, GameWorld* pWorld) {

    world                       = pWorld;
    gameObject                  = go;
    gravity                     = -9.8f;

    maxVelocity                 = 10.0f;
    runForce                    = 50.0f;
    groundOffset                = 0.1f;
    jumpForce                   = 3.0f;
    airMultiplier               = 1.0f;
    fastFallingMultiplier       = 1.1f;

    drag                        = 0.5f;

    isGrounded                  = false;
    isGrappling                 = false;
    isDashing                   = false;

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

    //DO IT THE OTHER WAY ROUND, GC SETS ISGRAPPLING HERE.
    //EVERY PLAYER WILL HAVE A PLAYERPHYSCOMPONENT BUT NOT THE OTHERWAY RIOUND SMILE.

    if(!isGrounded){
        airMultiplier = 0.3f;
    } else{
        airMultiplier = 1.0f;
    }

    auto physGameObj = gameObject->GetPhysicsObject();
    if(!isGrounded){
        physGameObj->AddForce(Vector3(0,gravity * 0.5f,0));
    }

//    GroundCheck(physGameObj, gameObject->GetTransform().GetPosition());

    MinimizeSlide(physGameObj);
    ClampPlayerVelocity(physGameObj);
//    FastFalling(physGameObj);

}

//multiply velocity instead of force since forces can be dangerous
void PlayerPhysComponent::FastFalling(PhysicsObject* physGameObj) {

    if(physGameObj->GetLinearVelocity().y <1.0f){
        physGameObj->SetLinearVelocity(Vector3(physGameObj->GetLinearVelocity().x,
                                               -abs(physGameObj->GetLinearVelocity().y) * fastFallingMultiplier,
                                               physGameObj->GetLinearVelocity().z));
    }
}

void PlayerPhysComponent::ClampPlayerVelocity(PhysicsObject* physGameObj) {
    if(isGrappling){
        return;
    }

    if(physGameObj->GetLinearVelocity().Length() > maxVelocity){

        auto curVel = physGameObj->GetLinearVelocity();
        curVel = curVel.Normalised();
        physGameObj->SetLinearVelocity(Vector3(curVel.x* maxVelocity ,physGameObj->GetLinearVelocity().y, curVel.z* maxVelocity));
    }
}

void PlayerPhysComponent::MinimizeSlide(PhysicsObject *physGameObj) {

    Vector3 currentVel = physGameObj->GetLinearVelocity();
    Vector3 invVel = currentVel * -1;
    invVel.y = 0;
    Vector3 normalisedInvVel = invVel.Normalised();

    if(isGrounded){
        physGameObj->SetLinearVelocity(currentVel + normalisedInvVel * drag);
    }
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

