//
// Created by idhant on 12-02-2024.
//
#include "PlayerPhysComponent.h"
using namespace NCL::CSC8503;

PlayerPhysComponent::PlayerPhysComponent(GameObject *go, GameWorld* pWorld) {

    world = pWorld;
    gameObject = go;

    maxVelocity = 10.0f;
    runForce = 50.0f;

    groundOffset = 0.1f;
    isGrounded = false;
    jumpForce = 1.0f;
    airMultiplier = 1.0f;

    fastFallingMultiplier = 1.1f;

}

void PlayerPhysComponent::ProcessMovementInput(Vector3 fwdAxis, Vector3 rightAxis, Vector2 playerInput) {

    if(isGrappling){
        return;
    }

    auto pGO = gameObject->GetPhysicsObject();

    pGO->AddForce(fwdAxis  *playerInput.y * runForce * airMultiplier);
    pGO->AddForce(rightAxis *playerInput.x * runForce * airMultiplier);

}

void PlayerPhysComponent::PhysicsUpdate(float dt) {

    GrappleComponent* gc;
    gameObject->TryGetComponent(gc);
    isGrappling = gc->isPlayerGrappling();

    if(isGrappling){
        return;
    }


    if(!isGrounded){
        airMultiplier = 0.3f;
    } else{
        airMultiplier = 1.0f;
    }

    auto physGameObj = gameObject->GetPhysicsObject();

    GroundCheck(physGameObj, gameObject->GetTransform().GetPosition());
//    MinimizeSlide(physGameObj);

    ClampPlayerVelocity(physGameObj);
//    FastFalling(physGameObj);

}




void PlayerPhysComponent::ProcessJumpInput(float jumpKeyPresed) {

    if(isGrappling){
        return;
    }

    if(jumpKeyPresed && checkIsGrounded()){
        gameObject->GetPhysicsObject()->ApplyLinearImpulse(Vector3{0,1,0} * jumpForce);
    }

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

    if(physGameObj->GetLinearVelocity().Length() > maxVelocity){

        auto curVel = physGameObj->GetLinearVelocity();
        curVel = curVel.Normalised();
        physGameObj->SetLinearVelocity(Vector3(curVel.x* maxVelocity ,physGameObj->GetLinearVelocity().y, curVel.z* maxVelocity));
    }
}

void PlayerPhysComponent::MinimizeSlide(PhysicsObject *physGameObj) {

    if(!InputListener::hasPlayerPressed()){
        physGameObj->SetLinearVelocity(Vector3(0,physGameObj->GetLinearVelocity().y,0));
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

}

