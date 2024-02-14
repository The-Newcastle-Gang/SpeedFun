//
// Created by idhant on 14-02-2024.
//

#ifndef SPEEDFUN_GRAPPLECOMPONENT_H
#define SPEEDFUN_GRAPPLECOMPONENT_H
#include "GameObject.h"
#include "Component.h"
#include "GameWorld.h"

namespace NCL::CSC8503{
    class GameObject;
    class GameWorld;
    class GrappleComponent : public Component {
    public:

        GrappleComponent(GameObject* go, GameWorld* world);
        void PhysicsUpdate(float dt)override;
        void ProcessMovementInput(Vector3 position);
        GameWorld* world;

    protected:


    };
}



#endif //SPEEDFUN_GRAPPLECOMPONENT_H
