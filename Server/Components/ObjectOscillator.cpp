#include "ObjectOscillator.h"
#include "CapsuleVolume.h"
#define TAU 6.283185

ObjectOscillator::ObjectOscillator(GameObject * go, float period, float distance, Vector3 direction, float cooldown, float waitDelay) {
    gameObject = go;
    initPosition = go->GetTransform().GetPosition();
    this->frequency = 1.0f/period;
    this->distance = distance;
    this->normalisedDirection = direction;
    this->cooldown = cooldown;
    this->waitDelay = waitDelay;

    phys = go->GetPhysicsObject();
}

void ObjectOscillator::OnCollisionEnter(GameObject* other) {
    if (other->GetTag() == PLAYER) {

        float topOfOscillator = gameObject->GetTransform().GetPosition().y + halfHeight;
        CapsuleVolume* capVolume = ((CapsuleVolume*)other->GetBoundingVolume());
        float playerFeet = other->GetTransform().GetPosition().y - capVolume->GetHalfHeight();
        if (playerFeet >= topOfOscillator)
        {
            objectsOnPlatform[other] = true;

        }
    }
}

void ObjectOscillator::OnCollisionEnd(GameObject* other) {
    if (other->GetTag() == PLAYER) {
        objectsOnPlatform[other] = false;
        other->GetPhysicsObject()->ApplyLinearImpulse(lastVelocity);
    }
}

void ObjectOscillator::PhysicsUpdate(float dt) {
    timer += dt;
    switch (state)
    {
    case ObjectOscillator::WAITING: {
        if (timer >= waitDelay) {
            float leftover = timer - waitDelay;
            state = RUNNING;
            timer = 0.5f * 1/frequency + leftover;
            isReturning = true;
            UpdateOscillation(dt);
        }
        break;
    }
    case ObjectOscillator::COOLDOWN: {
        if (timer >= cooldown) {
            float leftover = timer - cooldown;

            state = RUNNING;
            timer = 0.0f + leftover;
            UpdateOscillation(dt);
        }
        break;
    }

    case ObjectOscillator::RUNNING: {
        UpdateOscillation(dt);
        break;
    }
    default:
        break;
    }
}

void ObjectOscillator::UpdateOscillation(float dt) {
    if (timer * frequency >= 0.5f && !isReturning) {
        float leftover = timer - 0.5f * 1.0f/frequency;
        timer = leftover;
        state = WAITING;
        return;
    }
    if (timer * frequency >= 1.0f && isReturning) {
        float leftover = timer - 1/frequency;
        isReturning = false;
        timer = 0.0f + leftover;
        state = COOLDOWN;
        return;
    }

    Vector3 lastPos = gameObject->GetTransform().GetPosition();

    float cosTimer = (-1.0f * cos((timer * frequency * TAU)) + 1) * 0.5f; //this gets a value from 0 to 1 where 0 is the initial value 
    gameObject->GetTransform().SetPosition(initPosition + normalisedDirection * cosTimer * distance);


    Vector3 changeInPosition = (gameObject->GetTransform().GetPosition() - lastPos);
    lastVelocity = changeInPosition / dt;
    phys->SetLinearVelocity(lastVelocity); //do this so they can push things around!

    for (std::pair<GameObject*, bool> pair : objectsOnPlatform) {
        if (pair.second) {
            Transform* t = pair.first->GetTransformPointer();
            t->SetPosition(t->GetPosition() + changeInPosition); //subtract the velocity we added last update (cancel it out)
        }
    }

}
