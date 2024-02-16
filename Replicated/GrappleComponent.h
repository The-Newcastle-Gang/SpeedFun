//
// Created by idhant on 14-02-2024.
//

#ifndef SPEEDFUN_GRAPPLECOMPONENT_H
#define SPEEDFUN_GRAPPLECOMPONENT_H
#include "GameObject.h"
#include "Component.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "PlayerPhysComponent.h"

namespace NCL::CSC8503{
    class GameObject;
    class GameWorld;
    class GrappleComponent : public Component {
    public:

        GrappleComponent(GameObject* go, GameWorld* world);
        void PhysicsUpdate(float dt)override;
        void Update(float dt) override;
        void ProcessGrappleInput(float playerInput, Quaternion lookDir);


        bool isPlayerGrappling(){return isGrappling;}

    protected:
        void ExecuteGrapple(Vector3 GrapplePoint);

        GameWorld* world;
        float maxGrappleLen = 0.0f;
        bool isGrappling = false;
        bool canGrapple;
        Vector3 GrapplePoint;
        float time;
        float grappleCooldown = 0.0f;
        float airTime;
    private:
        float arcHeightOffset = 0.0f;
        int XZVelModifier = 0;
    };
}



#endif //SPEEDFUN_GRAPPLECOMPONENT_H
