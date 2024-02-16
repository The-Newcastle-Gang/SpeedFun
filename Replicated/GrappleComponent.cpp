//
// Created by idhant on 14-02-2024.
//

#include "GrappleComponent.h"
using namespace NCL::CSC8503;

GrappleComponent::GrappleComponent(GameObject* go, GameWorld* pWorld) : world(pWorld) {

    world = pWorld;
    gameObject = go;
    maxGrappleLen = 20.0f;
    time = 0.0f;
    canGrapple = true;
    grappleCooldown = 3.0f;
    arcHeightOffset = 10.0f;
    XZVelModifier = 7;
    airTime = 0;

}

void GrappleComponent::PhysicsUpdate(float dt) {


}

void GrappleComponent::Update(float dt) {
//    time += dt;
//    std::cout << time <<std::endl;
//    if (time > grappleCooldown && !canGrapple){
//        canGrapple = true;
//        time = 0.0f;
//    }

//this is a static value since our gravity stuff is kinda crappy so yeah.
    if(isGrappling){
        time +=dt;
        if(time> 0.3f){
            isGrappling = false;
            PlayerPhysComponent* p;
            gameObject->TryGetComponent(p);
            p->SetGrappling(false);
        }
    }


}

void GrappleComponent::ProcessGrappleInput(float playerInput, Quaternion rotation) {

    if(!playerInput){
        return;
    }
    //holy fucking shit openGL dude why are you z -1 im gonna krill you
    //my god (this furthered like all of my calculations for a good 45minutes)
    //im not kidding I redid my math only to realize that the fwd vector for opengl is z= -1


    Vector3 lookDirection = rotation * Vector3(0,0,-1);

    Ray ray = Ray(gameObject->GetTransform().GetPosition(),lookDirection);
    RayCollision closestColl;
    closestColl.rayDistance =  maxGrappleLen;

    if(world->Raycast(ray, closestColl, true, gameObject)){
        GrapplePoint = closestColl.collidedAt;
        if((gameObject->GetTransform().GetPosition() - GrapplePoint).Length() > maxGrappleLen){
            //play fake grapple anim
            return;
        }
        else{
            //play latched grapple anim or fire signal to client, play anim.
            ExecuteGrapple(closestColl.collidedAt);
        }
    }
}

//s = ut+ 1/2 at2

void GrappleComponent::ExecuteGrapple(Vector3 GrapplePoint) {

    if(!canGrapple) { return; }

    PlayerPhysComponent* p;
    gameObject->TryGetComponent(p);
    p->SetGrappling(true);
//    canGrapple = false;
    isGrappling = true;

    float gravity = -9.8 * 2;

    auto posCache = gameObject->GetTransform().GetPosition();
    Vector3 lowestPoint = Vector3(posCache.x, posCache.y - 1.0f, posCache.z);

    float height = (GrapplePoint.y - lowestPoint.y) + arcHeightOffset;

    float displacementY = GrapplePoint.y - posCache.y;


    Vector3 displacementXZ = Vector3(GrapplePoint.x - posCache.x, 0, GrapplePoint.z - posCache.z);

    float inAirTime =  (sqrt(-2 * height/gravity) + sqrt(2 * (displacementY - height)/gravity));

    Vector3 velocityY = Vector3(0,1,0) * sqrt((-2 * gravity * height));
    Vector3 velocityXZ = displacementXZ / inAirTime;


    //TODO:: THROW THIS IN A VECTOR3 FUNCTION
    if(velocityY.x != velocityY.x || velocityY.y != velocityY.y || velocityY.z != velocityY.z ||
        velocityXZ.x !=velocityXZ.x ||velocityXZ.y !=velocityXZ.y ||velocityXZ.z !=velocityXZ.z){
        return;
    }
    else {

        gameObject->GetPhysicsObject()->ClearForces();
        gameObject->GetPhysicsObject()->SetLinearVelocity(Vector3(velocityY + velocityXZ * XZVelModifier));
    }
}