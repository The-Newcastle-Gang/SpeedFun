//
// Created by idhant on 26-02-2024.
//

#include "StageTimer.h"

StageTimer::StageTimer() {
    std::cout << "Timer initialized" << std::endl;
    currentState = StageTimer::Paused;
    currentMedal = Medal::Default;
    elapsedTime = 0.0f;
    finalTime = 0.0f;
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


void StageTimer::EndTimer() {
    if(currentState != StageTimer::Paused) { currentState = StageTimer::Paused; }
    CalculateMedal();
//    CalculateScore();
//    ResetTimer();
}

void StageTimer::ResetTimer() {
    elapsedTime = 0.0f;
    currentState = StageTimer::Paused;

}

void StageTimer::CalculateMedal() {
    finalTime = elapsedTime;
    if (finalTime < 0 ) { std::cerr << "FinalTime is less than 0!\n"; return;}

    if( finalTime > 0 && finalTime < platinumTime )
    {
        currentMedal = Medal::Platinum;
        currentMedalColour = { 0.90f, 0.894f, 0.886f, 1.0f };
    }
    else if(finalTime > platinumTime && finalTime < goldTime)
    {
        currentMedal = Medal::Gold;
        currentMedalColour = { 0.788f, 0.69f, 0.216f, 1.0f};
    }
    else if(finalTime > goldTime && finalTime < silverTime)
    {
        currentMedal = Medal::Silver;
        currentMedalColour = { 0.843f, 0.843f, 0.843f, 1.0f};
    }
    else if(finalTime > silverTime)
    {
        currentMedal = Medal::Bronze;
        currentMedalColour = { 0.416f, 0.22f, 0.02f, 1.0f};
    }
}

void StageTimer::CalculateScore() {
    std::cout << "calcing score: ";
}
