//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_STAGESCORER_H
#define SPEEDFUN_STAGESCORER_H
#include <iostream>

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

    float GetFinalTime()          { return finalTime; }
    Medal GetCurrentMedal()       { return currentMedal; }

    void ResumeTimer(){ currentState = StageTimer::Running; }
    void PauseTimer() { currentState = StageTimer::Paused;  }

protected:

    void CalculateMedal();
    void CalculateScore();


    Medal       currentMedal;
    TimerState  currentState;

    float finalTime;
    float elapsedTime;

};


#endif //SPEEDFUN_STAGESCORER_H
