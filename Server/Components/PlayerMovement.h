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
#include "Replicated.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;



class PlayerMovement : public Component {
public:
    PlayerMovement(GameObject* g, GameWorld* w);
    void UpdateInputs(Vector3 pRightAxis, Vector2 pInputDirection, Quaternion pPlayerRotation);
    void ResetMovementInput() { inputDirection = { 0,0 }; }
    void Update(float dt) override;

    void Jump();
    void Grapple();
    void SetInAir();

    struct CameraAnimationCallData {
        bool jump = false;
        bool isGrounded = false;
        float land = 0.0f;
        float fallDistance = 0.0f;
        float groundMovement = 0.0f;
        float strafeSpeed = 0.0f;
        float shakeIntensity = 0.0f;
        int grapplingEvent = 0; //0 is nothing has happened, 1 is start grapple, 2 is end grapple.
        bool isGrappling = false;
    };

    CameraAnimationCallData cameraAnimationCalls;

    struct PlayerAnimationCallData {
        bool isGrappling = false;
        bool inAir = false;
        int strafe = 0;
        bool backwards = false;
        bool hasInput = false;
        bool isFalling = false;
    };

    entt::sink<entt::sigh<void(GameObject*, Vector3)>> GrappleStart;
    entt::sink<entt::sigh<void(GameObject*)>> GrappleEnd;
    entt::sink<entt::sigh<void(GameObject*, Vector3 position)>> GrappleUpdate;


    PlayerAnimationCallData playerAnimationCallData;

    struct UIAnimationData {
        int grapplingAvailability = -1;
    };

    UIAnimationData uiAnimationData;

    void ToggleDebug() { debugEnabled = !debugEnabled; }

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

    void InterruptGrapple();

private:
    GameWorld* world;

    struct MovementState {
        std::function<void()> OnStart;
        std::function<void(float)> UpdateState;
        std::function<void()> OnExit;
    };

    bool debugEnabled = false;

    bool hasCoyoteExpired;

    float coyoteTime;
    float coyoteTimeTimer;
    float runSpeed;
    float jumpVelocity;
    float dragFactor;
    float maxHorizontalVelocity;
    float maxVerticalVelocity;
    int jumpQueued;
    float fallApex = 0.0f;
    bool isFalling = false;


    GameObject* grappledObject;
    Vector3 deltaGrappledObject;
    Vector3 grapplePoint;

    MovementState ground;
    MovementState air;
    MovementState grapple;

    MovementState* activeState;

    Vector3 rightAxis;
    Vector2 inputDirection;
    Quaternion playerRotation;

    entt::sigh<void(GameObject*, Vector3)> onGrappleStart;
    entt::sigh<void(GameObject*)> onGrappleEnd;
    entt::sigh<void(GameObject*, Vector3 position)> onGrappleUpdate;

    bool GroundCheck();
    void StartInAir();
    void UpdateInAir(float dt);
    void LeaveInAir();
    void StartGround();
    void UpdateAnimDataFromInput();
    void UpdateOnGround(float dt);
    void LeaveGround();
    void GrappleSwinging(float dt);
    void SwitchToState(MovementState *state);

    void UpdateGrapple(float dt);
    void FireGrapple();

    void OnGrappleStart();

    void OnGrappleLeave();

    void OnGrappleUpdate(float dt);
};


#endif //SPEEDFUN_PLAYERMOVEMENT_H
