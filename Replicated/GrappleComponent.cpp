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
        ExecuteGrapple(closestColl.collidedAt);

        //execute grapple after a delay maybe?
//        ExecuteGrapple(test->GetTransform().GetPosition());

    }

}

//s = ut+ 1/2 at2

void GrappleComponent::ExecuteGrapple(Vector3 GrapplePoint) {
    isGrappling = true;

    float gravity = -9.8;


    auto posCache = gameObject->GetTransform().GetPosition();
    Vector3 lowestPoint = Vector3(posCache.x, posCache.y-1.0f, posCache.z);
    float height = (GrapplePoint.y - lowestPoint.y) + 5.0f;

    float displacementY = GrapplePoint.y - posCache.y;


    Vector3 displacementXZ = Vector3(GrapplePoint.x - posCache.x, 0, GrapplePoint.z - posCache.z);


    Vector3 velocityY =Vector3(0,1,0) *sqrt((-2 * gravity * height));
    Vector3 velocityXZ = displacementXZ / (sqrt(-2*height/gravity) + sqrt(2*(displacementY - height)/gravity));

    gameObject->GetPhysicsObject()->ClearForces();
    gameObject->GetPhysicsObject()->SetLinearVelocity(Vector3(velocityY + velocityXZ*4));

}


