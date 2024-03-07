//
// Created by idhant on 26-02-2024.
//

#ifndef SPEEDFUN_LEVELMANAGER_H
#define SPEEDFUN_LEVELMANAGER_H

#include <iostream>
#include "LevelReader.h"
#include "StageTimer.h"

class LevelManager {
public:
    LevelManager();
    void UpdateTimer(float dt);
    bool TryReadLevel(std::string levelSource);

    void StartStageTimer();
    void EndStageTimer();
    int GetCurrentMedal() const;

    std::shared_ptr<LevelReader> GetLevelReader() const { return levelReader; }

protected:

    std::shared_ptr<LevelReader> levelReader;
    std::unique_ptr<StageTimer> stageTimer;
    int currentTimer;

    struct {
        Vector3 currentStartPos;
        Vector3 currentEndPos;
        Vector3 currentDeathPos;

    } currentLevelDetails;

    std::vector<PrimitiveGameObject*> pList;

    int currentMedal;

};


#endif //SPEEDFUN_LEVELMANAGER_H
