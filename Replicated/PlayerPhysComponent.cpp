//
// Created by idhant on 12-02-2024.
//
#include "PlayerPhysComponent.h"
using namespace NCL::CSC8503;

PlayerPhysComponent::PlayerPhysComponent(GameObject *go, GameWorld* pWorld) {

    world = pWorld;
    gameObject = go;
//    runForce = 100.0f;
    maxVelocity = 10.0f;
    //drag =
    //airDrag =
    //calc
    fastFallingMultiplier = 1.4f;
}

void PlayerPhysComponent::PhysicsUpdate(float dt) {

    //THIS IS WHERE THE MAGIC HAPPENS
    auto physGameObj = gameObject->GetPhysicsObject();

    //can use this to set the values of each of the variables but since players will rarely be tweaked im setting them manually right now
    auto physMat= physGameObj->GetPhysMat();

    ClampPlayerVelocity(physGameObj);
    FastFalling(physGameObj);
    MinimizeSlide(physGameObj);

}

void PlayerPhysComponent::FastFalling(PhysicsObject* physGameObj) {

    if(physGameObj->GetLinearVelocity().y <1.0f){
        //multiply velocity instead of force since forces can be dangerous
        physGameObj->SetLinearVelocity(Vector3(physGameObj->GetLinearVelocity().x,
                                               physGameObj->GetLinearVelocity().y * fastFallingMultiplier,
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
        //lerp this to 0
        physGameObj->SetLinearVelocity(Vector3(0,physGameObj->GetLinearVelocity().y,0));
    }
}



