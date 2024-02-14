//
// Created by idhant on 14-02-2024.
//

#ifndef SPEEDFUN_GRAPPLECOMPONENT_H
#define SPEEDFUN_GRAPPLECOMPONENT_H
#include "GameObject.h"
#include "Component.h"
#include "GameWorld.h"
#include "PhysicsObject.h"

namespace NCL::CSC8503{
    class GameObject;
    class GameWorld;
    class GrappleComponent : public Component {
    public:

        GrappleComponent(GameObject* go, GameWorld* world);
        void PhysicsUpdate(float dt)override;
        void ProcessGrappleInput(float playerInput, Quaternion lookDir);
        void ExecuteGrapple(Vector3 GrapplePoint);

        bool isPlayerGrappling(){return isGrappling;}

    protected:
        GameWorld* world;
        float maxGrappleLen;
        bool isGrappling = false;

    };
}



#endif //SPEEDFUN_GRAPPLECOMPONENT_H
