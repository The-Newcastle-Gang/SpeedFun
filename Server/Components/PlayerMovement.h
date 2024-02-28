// Created by jdhyd on 2/19/2024.
//

#ifndef SPEEDFUN_PLAYERMOVEMENT_H
#define SPEEDFUN_PLAYERMOVEMENT_H

#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

class PlayerMovement : public Component {
public:
    PlayerMovement(GameObject* g, GameWorld* w);
    void UpdateInputs(Vector3 pRightAxis, Vector2 pInputDirection, Quaternion pPlayerRotation);
    void PhysicsUpdate(float fixedTime) override;
    void Update(float dt) override;

    void Jump();
    void Grapple();

private:
    GameWorld* world;

    struct MovementState {
        std::function<void()> OnStart;
        std::function<void(float)> UpdateState;
        std::function<void()> OnExit;
    };

    bool hasCoyoteExpired;

    float coyoteTime;
    float coyoteTimeTimer;
    float runSpeed;
    float jumpVelocity;
    float dragFactor;
    float maxHorizontalVelocity;
    int jumpQueued;

    struct GrappleInfo {
        Ray grappleRay;
        float travelDistance;
        float travelSpeed;
        float maxDistance;

        void SetActive(bool active) {
            isActive = active;
        }

        bool GetActive() {
            return isActive;
        }
    private:
        bool isActive;
    } grappleProjectileInfo;

    Vector3 grapplePoint;

    MovementState ground;
    MovementState air;
    MovementState grapple;

    MovementState* activeState;

    Vector3 rightAxis;
    Vector2 inputDirection;
    Quaternion playerRotation;

    bool GroundCheck();
    void StartInAir();
    void UpdateInAir(float dt);
    void LeaveInAir();
    void StartGround();
    void UpdateOnGround(float dt);
    void LeaveGround();
    void SwitchToState(MovementState *state);

    void UpdateGrapple(float dt);
    void FireGrapple();

    void OnGrappleStart();

    void OnGrappleLeave();

    void OnGrappleUpdate(float dt);
};


#endif //SPEEDFUN_PLAYERMOVEMENT_H
