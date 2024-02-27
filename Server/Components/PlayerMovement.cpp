//
// Created by jdhyd on 2/19/2024.
//

#include "PlayerMovement.h"
#include "CapsuleVolume.h"
#include "Ray.h"


PlayerMovement::PlayerMovement(GameObject *g, GameWorld *w) {
    world = w;
    gameObject = g;

    runSpeed = 5000.0f;
    jumpVelocity = 500.0f;

    dragFactor = 10.0f;
    coyoteTime = 0.5f;
    coyoteTimeTimer = coyoteTime;
    hasCoyoteExpired = false;

    grappleInfo.travelSpeed = 100.0f;
    grappleInfo.maxDistance = 5000.0f;
    grappleInfo.SetActive(false);

    maxHorizontalVelocity = 7.0f;

    rightAxis = Vector3();
    inputDirection = Vector2();

    ground.OnStart = [this](){ StartGround(); };
    ground.UpdateState = [this](float dt){ UpdateOnGround(dt); };
    ground.OnExit = [this]() { LeaveGround(); };

    air.UpdateState = [this](float dt){ UpdateInAir(dt); };
    air.OnExit = [this](){ LeaveInAir(); };
    air.OnStart = [this](){ StartInAir(); };

    activeState = &air;
}

void PlayerMovement::UpdateInputs(Vector3 pRightAxis, Vector2 pInputDirection, Quaternion pPlayerRotation) {
    rightAxis = pRightAxis;
    inputDirection = pInputDirection;
    playerRotation = pPlayerRotation;
}

void PlayerMovement::SwitchToState(MovementState* state) {
    activeState->OnExit();
    activeState = state;
    activeState->OnStart();
}

void PlayerMovement::StartInAir() {
    static float airRunSpeed = 1000.0f;
    static float airJumpVelocity = 0.0f;
    static float airDragFactor = 0.5f;
    static float airMaxHorizontalVelocity = 20.0f;

    runSpeed = airRunSpeed;
    jumpVelocity = airJumpVelocity;
    dragFactor = airDragFactor;
    maxHorizontalVelocity = airMaxHorizontalVelocity;
    cameraAnimationCalls.isGrounded = false;
}

void PlayerMovement::UpdateInAir(float dt) {
    if (GroundCheck()) {
        return SwitchToState(&ground);
    }
}

void PlayerMovement::LeaveInAir() {
    cameraAnimationCalls.land = true;
    cameraAnimationCalls.isGrounded = true;
}

void PlayerMovement::StartGround() {
    static float groundRunSpeed = 7000.0f;
    static float groundJumpVelocity = 7.0f;
    static float groundDragFactor = 8.5f;
    static float groundMaxHorizontalVelocity = 20.0f;

    runSpeed = groundRunSpeed;
    jumpVelocity = groundJumpVelocity;
    dragFactor = groundDragFactor;
    maxHorizontalVelocity = groundMaxHorizontalVelocity;

    hasCoyoteExpired = false;
    
}

void PlayerMovement::UpdateOnGround(float dt) {
    bool isGrounded = GroundCheck();
    if (!hasCoyoteExpired) {
        if (isGrounded) {
            coyoteTimeTimer = coyoteTime;
            hasCoyoteExpired = false;
        }
        else { coyoteTimeTimer -= dt; }

        if (coyoteTimeTimer <= 0.0f)hasCoyoteExpired = true;
    }

    if (!isGrounded && hasCoyoteExpired) {
        hasCoyoteExpired = false;
        return SwitchToState(&air); 
    }
}

void PlayerMovement::LeaveGround() {}


void PlayerMovement::PhysicsUpdate(float fixedTime) {

    // Add horizontal drag

    Vector3 dlinearVel = gameObject->GetPhysicsObject()->GetLinearVelocity();
    auto dhorizontalVel = Vector2(dlinearVel.x, dlinearVel.z);

    auto dDragHorizontalVel = (dhorizontalVel * -1) * fixedTime * dragFactor;
    dhorizontalVel += dDragHorizontalVel;
    gameObject->GetPhysicsObject()->SetLinearVelocity({dhorizontalVel.x, dlinearVel.y, dhorizontalVel.y});

    // Clamp max speed
    Vector3 linearVel = gameObject->GetPhysicsObject()->GetLinearVelocity();
    auto horizontalVel = Vector2(linearVel.x, linearVel.z);


    if (horizontalVel.Length() > maxHorizontalVelocity) {
        auto newHorizontalVel = horizontalVel.Normalised() * maxHorizontalVelocity;
        gameObject->GetPhysicsObject()->SetLinearVelocity(Vector3(newHorizontalVel.x, linearVel.y, newHorizontalVel.y));
    }
}

bool PlayerMovement::GroundCheck() {

    constexpr static float groundOffset = 0.1;

    auto physicsObject = gameObject->GetPhysicsObject();
    auto position = gameObject->GetTransform().GetPosition();

    auto collVol = (CapsuleVolume*)gameObject->GetBoundingVolume();
    Vector3 capBottom =  position - Vector3(0, collVol->GetHalfHeight(),0);
    capBottom += Vector3(0, groundOffset, 0);

    Ray ray = Ray(capBottom, Vector3(0,-1,0));
    RayCollision closestCollision;

    if (world->Raycast(ray, closestCollision, true, gameObject)) {
        Vector3 dist = closestCollision.collidedAt - capBottom;
        if (dist.Length() < groundOffset) {
            return true;
        }
    }

    return false;
}


void PlayerMovement::Update(float dt) {

    activeState->UpdateState(dt);
    UpdateGrapple(dt);

    auto fwdAxis = Vector3::Cross(Vector3(0,1,0), rightAxis);
    gameObject->GetPhysicsObject()->AddForce(fwdAxis * inputDirection.y * runSpeed * dt);
    gameObject->GetPhysicsObject()->AddForce(rightAxis * inputDirection.x * runSpeed * dt);
    cameraAnimationCalls.groundMovement = (gameObject->GetPhysicsObject()->GetLinearVelocity() * Vector3 (1,0,1)).Length();
    if (cameraAnimationCalls.groundMovement < 0.05f || activeState == &air) cameraAnimationCalls.groundMovement = 0.0f;
}

void PlayerMovement::Grapple() {

    Vector3 lookDirection = playerRotation.Normalised() * Vector3(0, 0, -1);
    grappleInfo.grappleRay = Ray(gameObject->GetTransform().GetPosition(), lookDirection);
    grappleInfo.travelDistance = 0;

    grappleInfo.SetActive(true);
}

void PlayerMovement::UpdateGrapple(float dt) {
    if (!grappleInfo.GetActive()) return;

    grappleInfo.travelDistance += grappleInfo.travelSpeed * dt;

    RayCollision collision;
    if (world->Raycast(grappleInfo.grappleRay, collision, true, gameObject)) {
        Vector3 grapplePoint = collision.collidedAt;
        if ((grappleInfo.grappleRay.GetPosition() - grapplePoint).Length() < grappleInfo.travelDistance) {
            FireGrapple(grapplePoint);
            grappleInfo.SetActive(false);
            return;
        }
    }

    auto dir = grappleInfo.grappleRay.GetDirection().Normalised() * grappleInfo.travelDistance;

    Debug::DrawLine(grappleInfo.grappleRay.GetPosition(), grappleInfo.grappleRay.GetPosition() + dir);


    if (grappleInfo.travelDistance >= grappleInfo.maxDistance) {
        grappleInfo.SetActive(false);
    }
}

void PlayerMovement::FireGrapple(Vector3 grapplePoint) {
    auto pos = gameObject->GetTransform().GetPosition();

    float gravity = -9.8;

    float displaceY = grapplePoint.y - pos.y;
    Vector3 displaceXZ = Vector3(grapplePoint.x - pos.x, 0, grapplePoint.z - pos.z);
    float inAirTime = sqrt(abs(-2 * displaceY / gravity));
    Vector3 velocityY = Vector3(0,1,0) * sqrt(abs((-2 * gravity* displaceY)));

    Vector3 velocityXZ = displaceXZ / inAirTime;
    if(displaceY < 0){
        velocityY *= -1 * 0.3f;
    }
    Vector3 totalVel = velocityY  + velocityXZ;
    gameObject->GetPhysicsObject()->ClearForces();
    gameObject->GetPhysicsObject()->SetLinearVelocity(totalVel * 2.0f);
}


void PlayerMovement::Jump() {
    hasCoyoteExpired = false;
    PhysicsObject* physOb = gameObject->GetPhysicsObject();

    Vector3 currentVelocity = physOb->GetLinearVelocity();
    gameObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + Vector3{0,jumpVelocity*0.01f,0}); //hacky way to allow us to directly set velocity
    physOb->SetLinearVelocity(currentVelocity + Vector3{ 0, 1, 0 } * jumpVelocity);
    if(activeState!=&air) cameraAnimationCalls.jump = true;
    SwitchToState(&air);
    
    
}

