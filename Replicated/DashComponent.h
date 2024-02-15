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
        void Update(float dt) override;
        void ProcessDashInput(bool dashButton, Quaternion rotation);

    protected:

        void ExecuteDash(Vector3 LookDirection);

        float dashStrength = 0.0f;
        float dashCooldown = 0.0f;
        float dashRecharge = 0.0f;
        int maxDashes = 0.0f;
        int currentDashAmt = 0.0f;
        bool canDash;
        bool isDashing;
        float time = 0.0f;

    };
}

#endif //SPEEDFUN_DASHCOMPONENT_H
