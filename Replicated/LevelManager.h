#ifndef GITIGNORE_LEVELMANGER_H
#define GITIGNORE_LEVELMANGER_H

#include <string>
#include <vector>
#include <iostream>

class LevelManager {
public:
    LevelManager();
    ~LevelManager();

    static std::string GetCurrentLevel() { return levelList[currentLevelIndex];}
    static void GoToLevelNumber(int level);
    static void GoToNextLevel();

private:
    static std::vector<std::string> levelList;
    static int currentLevelIndex;


};


#endif //GITIGNORE_LEVELMANGER_H
