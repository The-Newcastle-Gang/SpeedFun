//
// Created by idhant on 14-02-2024.
//

#ifndef SPEEDFUN_DASHCOMPONENT_H
#define SPEEDFUN_DASHCOMPONENT_H

#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"

namespace NCL::CSC8503 {
    class GameObject;
    class DashComponent : public Component {
    public:
        DashComponent(GameObject* go);
        void PhysicsUpdate(float dt) override;
        void ProcessDashInput(bool dashButton, Quaternion rotation);

    protected:

        void ExecuteDash(Vector3 LookDirection);

        float dashStrength;
        float dashCooldown;
        int maxDashes;
        int currentDashAmt;
        bool canDash;
        bool isDashing;

    };
}

#endif //SPEEDFUN_DASHCOMPONENT_H
