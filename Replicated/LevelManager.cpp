//
// Created by c3064585 on 16/02/2024.
//

#include "LevelManager.h"

int LevelManager::currentLevelIndex = 0;
std::vector<std::string> LevelManager::levelList = { "debuglvl",
                  "finaltest",
                  "level1",
                  "level2",
                  "level3",
                  "sillylevel" };


void LevelManager::GoToNextLevel() {
    if(currentLevelIndex < levelList.size() - 1){
        currentLevelIndex++;
    }
    else {
        std::cerr << "Level does not exist\n";
        currentLevelIndex = 0;
    }
}

