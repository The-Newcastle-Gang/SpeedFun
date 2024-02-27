//
// Created by idhant on 26-02-2024.
//

#include "LevelManager.h"
LevelManager::LevelManager() {
    std::cout << "initialize manager" << std::endl;
    levelReader = std::make_unique<LevelReader>();
    stageTimer = std::make_unique<StageTimer>();
    std::cout <<" level and stagetimer inits" <<std::endl;
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
    std::cout << stageTimer->GetCurrentMedal();
    std::cout <<stageTimer->GetFinalTime();

}

bool LevelManager::TryReadLevel(std::string levelSource) {
    std::cout << "Reading level" + levelSource + "\n";

    if (!levelReader->HasReadLevel(levelSource + ".json"))
    {
        std::cerr << "No file available. Check " + Assets::LEVELDIR << std::endl;
        return false;
    }



    return true;
}

std::vector<PrimitiveGameObject *> LevelManager::GetCurrentPrimitiveList() const {
    return levelReader->GetPrimitiveList();
}
