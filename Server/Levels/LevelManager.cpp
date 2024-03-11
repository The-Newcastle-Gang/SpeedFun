#include "LevelManager.h"
//
// Created by idhant on 26-02-2024.
//

#include "LevelManager.h"
LevelManager::LevelManager() {
    std::cout << "initialize manager" << std::endl;
    levelReader = std::make_unique<LevelReader>();
    stageTimer = std::make_unique<StageTimer>();
    std::cout <<" level and stagetimer inits" <<std::endl;
    currentMedal = 0;
}

void LevelManager::UpdateTimer(float dt) {
    stageTimer->Update(dt);
}

void LevelManager::StartStageTimer() {
    stageTimer->ResetTimer();
    stageTimer->ResumeTimer();
}

void LevelManager::EndStageTimer() {
    stageTimer->EndTimer();
}

bool LevelManager::TryReadLevel(std::string levelSource) {
    //std::cout << "Reading level" + levelSource + "\n";

    if (!levelReader->HasReadLevel(levelSource + ".json"))
    {
        std::cerr << "No file available. Check " + Assets::LEVELDIR << std::endl;
        return false;
    }
    return true;
}

int LevelManager::GetCurrentMedal() const {
    return stageTimer->GetCurrentMedal();
}
Vector4 LevelManager::GetCurrentMedalColour() const {
    return stageTimer->GetCurrentMedalColour();
}

void LevelManager::LoadLevelMap()
{
    levelMap.insert(std::pair<int, std::string>(TEST_LEVEL, "newTest"));
    levelMap.insert(std::pair<int, std::string>(LEVEL_1, "level1"));
}
