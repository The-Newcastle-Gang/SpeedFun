//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_LEVELMANAGER_H
#define SPEEDFUN_LEVELMANAGER_H
#define COUNTDOWN_MAX 5.0f

#include <iostream>
#include "LevelReader.h"
#include "StageTimer.h"

class LevelManager {
public:
    LevelManager();
    void UpdateTimer(float dt);
    bool TryReadLevel(std::string levelSource);

    bool UpdateCountdown(float dt);
    void ResetCountdown() { countdownTimer = COUNTDOWN_MAX; }
    float GetCountdown() { return countdownTimer; }

    void StartStageTimer();
    void EndStageTimer();

    void SetLevel(int level) { currentLevel = level; }
    [[no_discard]] int GetLevel() const { return currentLevel; }

    void Reset() {
        countdownTimer = COUNTDOWN_MAX;
        currentTimer = 0.0f;
        stageTimer->ResetTimer();
    }

    void ChangeLevel(int level);

    int GetCurrentMedal() const;
    Vector4 GetCurrentMedalColour() const;

    std::shared_ptr<LevelReader> GetLevelReader() const { return levelReader; }

protected:

    std::shared_ptr<LevelReader> levelReader;
    std::unique_ptr<StageTimer> stageTimer;
    float countdownTimer = COUNTDOWN_MAX;
    int currentTimer;

    struct {
        Vector3 currentStartPos;
        Vector3 currentEndPos;
        Vector3 currentDeathPos;

    } currentLevelDetails;

    int currentMedal;

    int currentLevel = 0;
};


#endif //SPEEDFUN_LEVELMANAGER_H
