//
// Created by idhant on 12-02-2024.
//
#include "PlayerPhysComponent.h"
using namespace NCL::CSC8503;

PlayerPhysComponent::PlayerPhysComponent(GameObject *go, GameWorld* pWorld) {

    world = pWorld;
    gameObject = go;
    maxVelocity = 10.0f;
    isGrounded = false;
    groundOffset = 0.9f;
    fastFallingMultiplier = 1.4f;

//    runForce = 100.0f;
    //drag =
    //airDrag =
    //calc
}

void PlayerPhysComponent::PhysicsUpdate(float dt) {


    //can use this to set the values of each of the variables but since players will rarely be tweaked im setting them manually right now
//    auto physMat= physGameObj->GetPhysMat();

    auto physGameObj = gameObject->GetPhysicsObject();
    ClampPlayerVelocity(physGameObj);
    FastFalling(physGameObj);
    MinimizeSlide(physGameObj);

    GroundCheck(physGameObj, gameObject->GetTransform().GetPosition());


}

void PlayerPhysComponent::FastFalling(PhysicsObject* physGameObj) {

    if(physGameObj->GetLinearVelocity().y <1.0f){
        //multiply velocity instead of force since forces can be dangerous
        physGameObj->SetLinearVelocity(Vector3(physGameObj->GetLinearVelocity().x,
                                               -abs(physGameObj->GetLinearVelocity().y) * fastFallingMultiplier,
                                               physGameObj->GetLinearVelocity().z));
    }
}

void PlayerPhysComponent::ClampPlayerVelocity(PhysicsObject* physGameObj) {

    if(physGameObj->GetLinearVelocity().Length() > 10.0f){

        auto curVel = physGameObj->GetLinearVelocity();
        physGameObj->SetLinearVelocity(Vector3(curVel.Normalised()* maxVelocity));
    }
}

void PlayerPhysComponent::MinimizeSlide(PhysicsObject *physGameObj) {
    if(!InputListener::hasPlayerPressed()){

        physGameObj->SetLinearVelocity(Vector3(0,physGameObj->GetLinearVelocity().y,0));

    }
}

void PlayerPhysComponent::GroundCheck(PhysicsObject *physGameObj, Vector3 position) {
    Ray ray = Ray(position, Vector3(0,-1,0));
    RayCollision closestCollision ;

    if(world->Raycast(ray, closestCollision, true, gameObject)){
        Vector3 disp = closestCollision.collidedAt - position;
//        if(disp.Length() < 1.0f){
//           std::cout << disp.Length();
//        }
    }

}

