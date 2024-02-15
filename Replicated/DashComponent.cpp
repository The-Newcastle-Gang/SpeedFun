//
// Created by idhant on 14-02-2024.
//

#include "DashComponent.h"
using namespace NCL::CSC8503;

DashComponent::DashComponent(GameObject *go) {
    gameObject = go;

    dashStrength    = 5.0f;
    dashCooldown    = 4.0f;
    maxDashes       = 2;
    currentDashAmt  = 1;
    canDash         = true;
    isDashing       = false;
}

void DashComponent::PhysicsUpdate(float dt) {
    //timer to reset dash and then add to current dash amount

    //track if can dash (so isgrounded and dash amount >1)


}

void DashComponent::ProcessDashInput(bool dashButton, Quaternion rotation) {
    if(!canDash || !dashButton){
        return;
    }

    if(dashButton && canDash){
        ExecuteDash(rotation * Vector3(0,0,-1));
    }
}

void DashComponent::ExecuteDash(Vector3 LookDirection) {
    gameObject->GetPhysicsObject()->ApplyLinearImpulse(LookDirection* dashStrength);
}