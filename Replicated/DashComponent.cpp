//
// Created by idhant on 14-02-2024.
//

#include "DashComponent.h"
using namespace NCL::CSC8503;

DashComponent::DashComponent(GameObject *go) {
    gameObject = go;

    dashStrength    = 5.0f;
    dashCooldown    = 4.0f;
    dashRecharge    = 8.0f;
    maxDashes       = 2;
    currentDashAmt  = maxDashes;
    canDash         = true;
    isDashing       = false;
    time = 0.0f;
}

void DashComponent::PhysicsUpdate(float dt) {
    //timer to reset dash and then add to current dash amount

    //track if can dash (so isgrounded and dash amount >1)


}

void DashComponent::Update(float dt) {
    time += dt;
    if (time > dashCooldown && !canDash){
        time = 0.0f;
        canDash = true;
        std::cout << "Can dash!\n";
    }
    if (time > dashRecharge && currentDashAmt < maxDashes){
        time = 0.0f;
        currentDashAmt = maxDashes;
        std::cout << "Dash charges restored\n";
    }
}

void DashComponent::ProcessDashInput(bool dashButton, Quaternion rotation) {
    if(!canDash || !dashButton){
        return;
    }

    if(dashButton && canDash /*&& currentDashAmt < 0*/){
        ExecuteDash(rotation * Vector3(0,0,-1));
    }
}

void DashComponent::ExecuteDash(Vector3 LookDirection) {
    std::cout << "Execute dash!\n";
    currentDashAmt--;
    gameObject->GetPhysicsObject()->ClearForces();
    gameObject->GetPhysicsObject()->ApplyLinearImpulse(LookDirection* dashStrength);
}