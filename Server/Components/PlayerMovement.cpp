//
// Created by jdhyd on 2/19/2024.
//

#include "PlayerMovement.h"
#include "CapsuleVolume.h"


PlayerMovement::PlayerMovement(GameObject *g, GameWorld *w) {
    world = w;
    gameObject = g;

    runSpeed = 5000.0f;
    jumpForce = 500.0f;
    dragFactor = 9.0f;

    maxHorizontalVelocity = 7.0f;

    rightAxis = Vector3();
    inputDirection = Vector2();

}

void PlayerMovement::UpdateInputs(Vector3 pRightAxis, Vector2 pInputDirection) {
    rightAxis = pRightAxis;
    inputDirection = pInputDirection;
}


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

void PlayerMovement::GroundCheck() {

    auto position = gameObject->GetTransform().GetPosition();

    auto collVol = (CapsuleVolume*)gameObject->GetBoundingVolume();
    Vector3 capBottom =  position - Vector3(0, collVol->GetHalfHeight(),0);
    capBottom += Vector3(0, groundOffset, 0);

    Ray ray = Ray(capBottom, Vector3(0,-1,0));
    RayCollision closestCollision ;

    float groundEpsilon = 0.01;

    if(world->Raycast(ray, closestCollision, true, gameObject)){
        Vector3 disp = closestCollision.collidedAt - capBottom;

        if(disp.Length() < groundOffset + groundEpsilon){
            isGrounded = true;
        }
        else {
            isGrounded = false;
        }
    }
    else {
        isGrounded = false;
    }
}

void PlayerMovement::Update(float dt) {
    auto fwdAxis = Vector3::Cross(Vector3(0,1,0), rightAxis);
    gameObject->GetPhysicsObject()->AddForce(fwdAxis * inputDirection.y * runSpeed * dt);
    gameObject->GetPhysicsObject()->AddForce(rightAxis * inputDirection.x * runSpeed * dt);

}

void PlayerMovement::Jump() {
    gameObject->GetPhysicsObject()->AddForce(Vector3{0,1,0} * jumpForce);
}
