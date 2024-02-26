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
    static void Update(float dt);
    static void PauseTimer();
    static void EndTimer();
    static void ResetTimer();

    static float GetFinalTime()          { return finalTime; }
    static Medal GetCurrentMedal()       { return currentMedal; }

protected:

    static void CalculateMedal();
    static void CalculateScore();


    static Medal       currentMedal;
    static TimerState  currentState;

    static float finalTime;
    static float elapsedTime;

};


#endif //SPEEDFUN_STAGESCORER_H
