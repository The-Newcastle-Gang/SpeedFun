//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_LEVELMANAGER_H
#define SPEEDFUN_LEVELMANAGER_H
#define COUNTDOWN_MAX 5.0f

#include <iostream>
#include "LevelReader.h"
#include "StageTimer.h"

enum LEVELS
{
    TEST_LEVEL,
    LEVEL_1,
    LEVEL_2,
    MAX_LEVELS
};

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

    int GetCurrentMedal() const;
    Vector4 GetCurrentMedalColour() const;

    std::map<int, std::string> GetLevelMap() { return levelMap; }

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

    std::vector<PrimitiveGameObject*> pList;

    int currentMedal;

    const std::map<int, std::string> levelMap = {
        { TEST_LEVEL, "newTest" },
        { LEVEL_1, "dbtest" }
    };

};


#endif //SPEEDFUN_LEVELMANAGER_H
