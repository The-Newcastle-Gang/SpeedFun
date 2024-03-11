//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_STAGESCORER_H
#define SPEEDFUN_STAGESCORER_H
#include <iostream>
#include "Vector4.h"

class StageTimer {
    enum TimerState {
        Running,
        Paused
    };

    enum Medal{
        Platinum,
        Gold,
        Silver,
        Bronze,
        Default
    };

public:
    StageTimer();
    void Update(float dt);
    void EndTimer();
    void ResetTimer();

    float GetFinalTime() const            { return finalTime; }
    Medal GetCurrentMedal() const         { return currentMedal; }
    NCL::Maths::Vector4 GetCurrentMedalColour() const { return currentMedalColour; }

    void ResumeTimer(){ currentState = StageTimer::Running; }
    void PauseTimer() { currentState = StageTimer::Paused;  }

    float GetElapsedTime() { return elapsedTime; }
protected:

    void CalculateMedal();
    void CalculateScore();

    float platinumTime = 15.0f;
    float goldTime = 20.0f;
    float silverTime = 25.0f;


    Medal                   currentMedal;
    NCL::Maths::Vector4     currentMedalColour;
    TimerState              currentState;

    float finalTime;
    float elapsedTime;
};


#endif //SPEEDFUN_STAGESCORER_H
