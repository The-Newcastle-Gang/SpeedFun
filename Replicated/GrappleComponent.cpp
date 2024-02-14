//
// Created by idhant on 14-02-2024.
//

#include "GrappleComponent.h"
using namespace NCL::CSC8503;

GrappleComponent::GrappleComponent(GameObject* go, GameWorld* pWorld) : world(pWorld) {

    world = pWorld;
    gameObject = go;
    maxGrappleLen = 10.0f;

}

void GrappleComponent::PhysicsUpdate(float dt) {
}

void GrappleComponent::ProcessGrappleInput(float playerInput, Quaternion rotation) {

    if(!playerInput){
        return;
    }
    //holy fucking shit openGL dude why are you z -1 im gonna krill you
    //my god
    
    Vector3 lookDirection = rotation * Vector3(0,0,-1);

    Ray ray = Ray(gameObject->GetTransform().GetPosition(),lookDirection);
    RayCollision closestColl;
    closestColl.rayDistance =  maxGrappleLen;

    if(world->Raycast(ray, closestColl, true, gameObject)){
        auto test  = (GameObject*)closestColl.node;
        gameObject->GetTransform().SetPosition(test->GetTransform().GetPosition() + Vector3(0,4,0));
    }



}
