//
// Created by idhant on 26-02-2024.
//

#include "StageTimer.h"

StageTimer::StageTimer() {
    elapsedTime = 0.0f;
}

void StageTimer::Update(float dt) {
    switch (currentState) {
        case StageTimer::Running: {
            elapsedTime += dt;
            break;
        }
        case StageTimer::Paused: {
            break;
        }
    }
}

void StageTimer::PauseTimer() {
    currentState = StageTimer::Paused;
}

void StageTimer::EndTimer() {
    CalculateMedal();
    CalculateScore();
//    ResetTimer();
}

void StageTimer::ResetTimer() {
    elapsedTime = 0.0f;
    currentState = StageTimer::Paused;

}

void StageTimer::CalculateMedal() {
    std::cout << "calcing medal";
}

void StageTimer::CalculateScore() {
    std::cout << "calcing score";
}
