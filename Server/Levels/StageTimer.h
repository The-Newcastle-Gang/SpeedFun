//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_STAGESCORER_H
#define SPEEDFUN_STAGESCORER_H
#include <iostream>
#include "Vector4.h"
#include "Vector3.h"

enum Medal {
    Platinum = 0,
    Gold = 1,
    Silver = 2,
    Bronze = 3,
    Default = 4
};

class StageTimer {
    enum TimerState {
        Running,
        Paused
    };

public:
    StageTimer();
    void Update(float dt);
    void EndTimer();
    void ResetTimer();

    float GetElapsedTime() { return elapsedTime; }

    float GetFinalTime() const            { return finalTime; }
    Medal GetCurrentMedal() { CalculateMedal(); return currentMedal; } //we need to 
    NCL::Maths::Vector4 GetCurrentMedalColour() { CalculateMedal(); return currentMedalColour; }

    void ResumeTimer(){ currentState = StageTimer::Running; }
    void PauseTimer() { currentState = StageTimer::Paused;  }

    float GetPlatinumTime() { return platinumTime; }
    float GetGoldTime() { return goldTime; }
    float GetSilverTime() { return silverTime; }

    void SetMedalValues(NCL::Maths::Vector3 medals) {
        platinumTime = medals.x;
        goldTime = medals.y;
        silverTime = medals.z;
    }

    Medal GetCurrentMedal(float timeValue);
protected:

    void CalculateMedal();

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
