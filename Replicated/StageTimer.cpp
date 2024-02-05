#include "StageTimer.h"
#include "Debug.h"
using namespace std;
using namespace NCL;
StageTimer::StageTimer(double goldTime, double silverTime, double bronzeTime)
    : goldTimePeriod(goldTime), silverTimePeriod(silverTime), bronzeTimePeriod(bronzeTime), playerTime(0.0), isPlaying(false) {
    startTime = std::chrono::high_resolution_clock::now();

}
    void StageTimer::ShowTimer() {
        if (isPlaying&&!isRestart) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
            double totalTimeSeconds = elapsedSeconds.count();
            Debug::Print("Time: " + std::to_string(totalTimeSeconds) + " s", Vector2(5, 20), Debug::RED);
        }
        if (!isPlaying&&!isRestart) {
            std::chrono::duration<double> elapsedSeconds = endTime - startTime;
            double totalTimeSeconds = elapsedSeconds.count();
            Debug::Print("Time: " + std::to_string(totalTimeSeconds) + " s", Vector2(5, 20), Debug::RED);
        }
        if (isRestart&&!isPlaying) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
            double totalTimeSeconds = elapsedSeconds.count();
            Debug::Print("Time: " + std::to_string(totalTimeSeconds) + " s", Vector2(5, 20), Debug::RED);
        }
        }


    void StageTimer::Start() {
        if (!isPlaying&&!isRestart) {
            startTime = std::chrono::high_resolution_clock::now();
            isPlaying = true;
            isRestart = false;
        }
    }

    void StageTimer::Stop() {
        if (isPlaying||isRestart) {
            endTime = std::chrono::high_resolution_clock::now();
            isPlaying = false;
            isRestart = false;
        }
    }
    
    void StageTimer::Restart() {
        if (!isRestart&&!isPlaying) {
            restartTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double>restartWaitingPeriod = restartTime - endTime;
            NCL::StageTimer::Timepoint restartWaitingPeriodCast = std::chrono::time_point_cast<NCL::StageTimer::Timepoint::duration>(startTime + restartWaitingPeriod);
            startTime = restartWaitingPeriodCast;
            isRestart = true;
            isPlaying = false;
        }
    }




    void StageTimer::SetPlayerTime(double time) {
        playerTime = time;
    }

    void StageTimer::ReturnScore() {
        if (playerTime <= goldTimePeriod) {
              Debug::Print("You got gold", Vector2(35, 50), Debug::RED);
        }
        else if (playerTime <= silverTimePeriod) {
              Debug::Print("You got silver", Vector2(35, 50), Debug::RED);
        }
        else if (playerTime <= bronzeTimePeriod) {
              Debug::Print("You got bronze", Vector2(35, 50), Debug::RED);
        }
        else {
              Debug::Print("You got bronze", Vector2(35, 50), Debug::RED);
        }
    }#include <StageTimer.h>
