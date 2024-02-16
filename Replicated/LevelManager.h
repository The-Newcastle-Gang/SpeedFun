#ifndef GITIGNORE_LEVELMANGER_H
#define GITIGNORE_LEVELMANGER_H

#include <string>

class LevelManager {
public:
    LevelManager();
    ~LevelManager();

//    std::string GetCurrentLevel() { return levelList[currentLevelIndex];}
    void GoToLevelNumber(int level);
    void GoToNextLevel();

private:
//    std::vector<std::string> levelList;
    int currentLevelIndex = 0;


};


#endif //GITIGNORE_LEVELMANGER_H
