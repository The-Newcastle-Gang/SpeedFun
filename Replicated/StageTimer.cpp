#include "StageTimer.h"
#include "Debug.h"
using namespace NCL;
StageTimer::StageTimer()
    :isPlaying(false) {
    startTime = 0.0;
    elapsedSeconds = 0.0;
}
    void StageTimer::ShowTimer(float dt) {
        if (isPlaying||isContinue) {
            elapsedSeconds += dt;
            Debug::Print("Time: " + std::to_string(elapsedSeconds) + " s", Vector2(5, 20), Debug::RED);
        }
        if (!isPlaying&&!isContinue) {
            Debug::Print("Time: " + std::to_string(elapsedSeconds) + " s", Vector2(5, 20), Debug::RED);
        }

     }


    void StageTimer::Start(float dt) {
        if (!isPlaying&&!isContinue) {
            startTime = dt;
            elapsedSeconds = 0.0;
            isPlaying = true;
            isContinue = false;
        }
    }

    void StageTimer::Stop(float dt) {
        if (isPlaying||isContinue) {
            isPlaying = false;
            isContinue = false;
        }
    }
    
    void StageTimer::Continue(float dt) {
        if (!isContinue && !isPlaying) {
            isContinue = true;
            isPlaying = false;
        }
    }
