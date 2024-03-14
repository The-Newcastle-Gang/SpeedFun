//
// Created by idhant on 26-02-2024.
//

#include "StageTimer.h"
#include "Replicated.h"

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

    switch (GetCurrentMedal(finalTime))
    {
        case(Medal::Platinum):
        
            currentMedal = Medal::Platinum;
            currentMedalColour = Replicated::PLATINUM;
            break;
        
        case(Medal::Gold):
        
            currentMedal = Medal::Gold;
            currentMedalColour = Replicated::GOLD;
            break;
        
        case(Medal::Silver):
        
            currentMedal = Medal::Silver;
            currentMedalColour = Replicated::SILVER;
            break;
        
        case(Medal::Bronze):
        
            currentMedal = Medal::Bronze;
            currentMedalColour = Replicated::BRONZE;
            break;
        
    }
}

Medal StageTimer::GetCurrentMedal(float timeValue) {
    if (timeValue >= 0 && timeValue < platinumTime)
    {
        return Medal::Platinum;
    }
    if (timeValue > platinumTime && timeValue < goldTime)
    {
        return Medal::Gold;
    }
    if (timeValue > goldTime && timeValue < silverTime)
    {
        return Medal::Silver;
    }
    if (timeValue > silverTime)
    {
        return Medal::Bronze;
    }
}
void StageTimer::CalculateScore() {
    std::cout << "calcing score: ";
}
