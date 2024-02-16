//
// Created by idhant on 16-02-2024.
//

#include "GroundedState.h"
using namespace NCL;
using namespace CSC8503;

GroundedState::GroundedState(PhysicsObject* physObj) {
    std::cout << "Default constructor for grounded called";

    playerPhysObj = physObj;
    maxVelocity = 10.0f;
    groundDrag  = 0.5f;

}

void GroundedState::OnEnter() {
    std::cout << "On Grounded enter \n";
}

void GroundedState::Update(float dt) {

    MinimizePlayerSlide(playerPhysObj);
//    ClampPlayerVelocity(playerPhysObj);

}

void GroundedState::MinimizePlayerSlide(PhysicsObject *physObj) {
    Vector3 currentVel = physObj->GetLinearVelocity();
    Vector3 invVel = currentVel * -1;
    invVel.y = 0;
    Vector3 normalisedInvVel = invVel.Normalised();
    physObj->SetLinearVelocity(currentVel + normalisedInvVel * groundDrag);

}

void GroundedState::ClampPlayerVelocity(PhysicsObject *physObj) {
    if(physObj->GetLinearVelocity().Length() > maxVelocity){

        auto curVel = physObj->GetLinearVelocity();
        curVel = curVel.Normalised();
        physObj->SetLinearVelocity(Vector3(curVel.x* maxVelocity ,physObj->GetLinearVelocity().y, curVel.z* maxVelocity));
    }
}

