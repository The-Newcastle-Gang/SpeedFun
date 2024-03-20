//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_LEVELMANAGER_H
#define SPEEDFUN_LEVELMANAGER_H
#define COUNTDOWN_MAX 5.0f
#define END_OF_LEVEL_MAX 5.0f

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

    bool UpdateEndOfLevelTimer(float dt);
    void ResetEndOfLevelTimer() { endOfLevelTimer = END_OF_LEVEL_MAX; }
    float GetEndOfLevelTimer() { return endOfLevelTimer; }

    void StartStageTimer();
    void EndStageTimer();
    float GetCurrentStageTime() { return stageTimer->GetElapsedTime(); }

    void SetLevel(int level) { currentLevel = level; }
    [[no_discard]] int GetLevel() const { return currentLevel; }

    void Reset() {
        ResetCountdown();
        ResetEndOfLevelTimer();
        stageTimer->ResetTimer();
    }

    int GetTotalLevels() { return totalLevels; }
    int GetAndSetNextLevel() { SetCurrentLevel((currentLevel + 1) % totalLevels); return currentLevel; }
    void SetCurrentLevel(int level) { currentLevel = level; }
    void ChangeLevel(int level);

    void SetHasReceivedLevel(bool hasReceived) { hasReceivedLevelFromServer = hasReceived; }
    bool GetHasReceivedLevel() { return hasReceivedLevelFromServer; }

    float GetElapsedTime() { return stageTimer->GetElapsedTime(); }
    float GetPlatinumTime() { return stageTimer->GetPlatinumTime(); }
    float GetGoldTime() { return stageTimer->GetGoldTime(); }
    float GetSilverTime() { return stageTimer->GetSilverTime(); }
    Medal GetCurrentMedal() { return stageTimer->GetCurrentMedal(stageTimer->GetElapsedTime()); }

    int GetCurrentMedal() const;
    Vector4 GetCurrentMedalColour() const;

    std::string GetLevelName(int i) {
        if (i < 0 || i >= totalLevels) return "";
        return levelReader->GetLevelName(i);
    }

    std::shared_ptr<LevelReader> GetLevelReader() const { return levelReader; }

protected:

    std::shared_ptr<LevelReader> levelReader;
    std::unique_ptr<StageTimer> stageTimer;
    float countdownTimer = COUNTDOWN_MAX;
    float endOfLevelTimer = END_OF_LEVEL_MAX;

    struct {
        Vector3 currentStartPos;
        Vector3 currentEndPos;
        Vector3 currentDeathPos;

    } currentLevelDetails;

    int currentMedal;

    int currentLevel = 0;
    int totalLevels = 0;

    bool hasReceivedLevelFromServer = false;
};


#endif //SPEEDFUN_LEVELMANAGER_H
