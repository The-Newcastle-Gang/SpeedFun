#include "ObjectOscillator.h"
#define TAU 6.283185

ObjectOscillator::ObjectOscillator(GameObject * go, float period, float distance, Vector3 direction, float cooldown, float waitDelay) {
    gameObject = go;
    initPosition = go->GetTransform().GetPosition();
    this->oneOverPeriod = 1.0f/period;
    this->distance = distance;
    this->direction = direction;
    this->cooldown = cooldown;
    this->waitDelay = waitDelay;
}

void ObjectOscillator::Update(float dt) {
    timer += dt;
    switch (state)
    {
    case ObjectOscillator::WAITING: {
        if (timer >= waitDelay) {
            float leftover = timer - waitDelay;
            state = RUNNING;
            timer = 0.5f * 1/oneOverPeriod + leftover;
            isReturning = true;
            UpdateOscillation();
        }
        break;
    }
    case ObjectOscillator::COOLDOWN: {
        if (timer >= cooldown) {
            float leftover = timer - cooldown;

            state = RUNNING;
            timer = 0.0f + leftover;
            UpdateOscillation();
        }
        break;
    }
    running:
    case ObjectOscillator::RUNNING: {
        UpdateOscillation();
        break;
    }

    default:
        break;
    }
}

void ObjectOscillator::UpdateOscillation() {
    if (timer * oneOverPeriod >= 0.5f && !isReturning) {
        float leftover = timer - 0.5f * 1.0f/oneOverPeriod;
        timer = leftover;
        state = WAITING;
        return;
    }
    if (timer * oneOverPeriod >= 1.0f && isReturning) {
        float leftover = timer - 1/oneOverPeriod;
        isReturning = false;
        timer = 0.0f + leftover;
        state = COOLDOWN;
        return;
    }

    float cosTimer = (-1.0f * cos((timer * oneOverPeriod * TAU)) + 1) * 0.5f; //this gets a value from 0 to 1 where 0 is the initial value 
    gameObject->GetTransform().SetPosition(initPosition + direction * cosTimer * distance);
}