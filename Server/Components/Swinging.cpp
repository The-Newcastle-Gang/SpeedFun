#include "Swinging.h"
#define TAU 6.283185

Swinging::Swinging(GameObject * go, float period, float cooldown, float waitDelay, float radius, bool changeAxis, bool changeDirection) {
    gameObject = go;
    initPosition = go->GetTransform().GetPosition();
    this->frequency = 1.0f/period;
    this->cooldown = cooldown;
    this->waitDelay = waitDelay;
    this->radius = radius;
    this->changeAxis = changeAxis;
    this->changeDirection = changeDirection;

    phys = go->GetPhysicsObject();
}

void Swinging::OnCollisionEnter(GameObject* other) {
    if (other->GetTag() == PLAYER) {
        std::cout << "HIT THE SWINGING BLOCK\n";
    }
}

void Swinging::OnCollisionEnd(GameObject* other) {
    if (other->GetTag() == PLAYER) {
        std::cout << "LEFT THE SWINGING BLOCK\n";
    }
}

void Swinging::PhysicsUpdate(float dt) {
    timer += dt;
    switch (state)
    {
    case Swinging::WAITING: {
        if (timer >= waitDelay) {
            float leftover = timer - waitDelay;
            state = RUNNING;
            timer = 0.5f * 1 / frequency + leftover;
            isReturning = true;
            UpdateOscillation(dt);
        }
        break;
    }
    case Swinging::COOLDOWN: {
        if (timer >= cooldown) {
            float leftover = timer - cooldown;

            state = RUNNING;
            timer = 0.0f + leftover;
            UpdateOscillation(dt);
        }
        break;
    }
                               running:
    case Swinging::RUNNING: {
        UpdateOscillation(dt);
        break;
    }

    default:
        break;
    }
}

void Swinging::UpdateOscillation(float dt) {
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

    float cosTimer = cos((timer * frequency * TAU)); //this gets a value from 0 to 1 where 0 is the initial value
    float sinTimer = sin((timer * frequency * TAU));

    if (changeDirection) cosTimer *= -1;

    Vector3 tempPos = initPosition;
    tempPos.y = initPosition.y + sinTimer * radius;

    switch (changeAxis)
    {
    case false:
        tempPos.z = initPosition.z + cosTimer * radius;
        break;
    case true:
        tempPos.x = initPosition.z + cosTimer * radius;
    }


    gameObject->GetTransform().SetPosition(tempPos);

}
