#ifndef GITIGNORE_LEVELMANGER_H
#define GITIGNORE_LEVELMANGER_H

using namespace std;

class LevelManager {
public:
    LevelManager();
    ~LevelManager();

    string GetCurrentLevel() { return levelList[currentLevelIndex];}
    void GoToLevelNumber(int level);
    void GoToNextLevel();

private:
    vector<string> levelList;
    int currentLevelIndex = 0;


};


#endif //GITIGNORE_LEVELMANGER_H
