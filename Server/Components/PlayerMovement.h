//
// Created by jdhyd on 2/19/2024.
//

#ifndef SPEEDFUN_PLAYERMOVEMENT_H
#define SPEEDFUN_PLAYERMOVEMENT_H

#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

class PlayerMovement : public Component {
public:
    PlayerMovement(GameObject* g, GameWorld* w);
    void UpdateInputs(Vector3 pRightAxis, Vector2 pInputDirection);
    void PhysicsUpdate(float fixedTime) override;
    void Update(float dt) override;

    void Jump();

private:
    GameWorld* world;
    struct MovementState {
        std::function<void()> OnStart;
        std::function<void(float)> CheckMove;
        std::function<void()> OnExit;
    };

    float runSpeed;
    float jumpForce;
    float dragFactor;
    float maxHorizontalVelocity;
    int jumpQueued;

    Vector3 rightAxis;
    Vector2 inputDirection;

    void GroundCheck();
};


#endif //SPEEDFUN_PLAYERMOVEMENT_H
