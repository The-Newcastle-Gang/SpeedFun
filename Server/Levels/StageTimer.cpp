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

    if( finalTime > 0 && finalTime < 15.0f )
    {
        currentMedal = Medal::Platinum;
    }
    else if(finalTime > 15.0f && finalTime < 20.0f)
    {
        currentMedal = Medal::Gold;
    }
    else if(finalTime > 20.0f && finalTime < 25.0f)
    {
        currentMedal = Medal::Silver;
    }
    else if(finalTime > 25.0f)
    {
        currentMedal = Medal::Bronze;
    }
    std::cout << "Medal: " << currentMedal << std::endl;
}

void StageTimer::CalculateScore() {
    std::cout << "calcing score: ";
}
