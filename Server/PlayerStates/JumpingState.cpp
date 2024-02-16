//
// Created by idhant on 16-02-2024.
//
#include "JumpingState.h"

using namespace NCL;
using namespace CSC8503;

JumpingState::JumpingState(PhysicsObject* physObj) {
    std::cout << "Default constructor for in air called";

    playerPhysObj = physObj;
    fastFallingMultiplier = 1.1f;
}

void JumpingState::OnEnter() {
    std::cout << "On jumping enter \n";
}

void JumpingState::Update(float dt) {
//    std::cout << "Update air";
//    FastFalling(playerPhysObj);

}

void JumpingState::FastFalling (PhysicsObject* physObj) {

    if(physObj->GetLinearVelocity().y <1.0f){
        physObj->SetLinearVelocity(Vector3(physObj->GetLinearVelocity().x,
                                               -abs(physObj->GetLinearVelocity().y) * fastFallingMultiplier,
                                               physObj->GetLinearVelocity().z));
    }

}