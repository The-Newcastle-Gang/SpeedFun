//
// Created by c3064585 on 16/02/2024.
//

#include "LevelManager.h"

LevelManager::LevelManager() {
    levelList = { "debuglvl",
                  "finaltest",
                  "level1",
                  "level2",
                  "level3",
                  "sillylevel" };

    // Level zero is debug, set this to 1 at the start of the game.
    currentLevelIndex = 0;
}

void LevelManager::GoToNextLevel() {
    if(currentLevelIndex < levelList.size() - 1){
        currentLevelIndex++;
    }
    else {
        std::cerr << "Level does not exist\n";
        currentLevelIndex = 0;
    }
}

