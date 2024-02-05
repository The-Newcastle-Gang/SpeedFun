#pragma once
#include <chrono>
namespace NCL {
    class StageTimer {
    public:
        StageTimer(double goldTime, double silverTime, double bronzeTime);
        void Start();
        void Stop();
        void Restart();
        void SetPlayerTime(double time);
        void ShowTimer();
        void ReturnScore();

    private:
        typedef std::chrono::time_point<std::chrono::high_resolution_clock> Timepoint;
        Timepoint startTime;
        Timepoint endTime;
        Timepoint restartTime;
        Timepoint restartWaitingPeriod;
        double goldTimePeriod;
        double silverTimePeriod;
        double bronzeTimePeriod;
        double totalTimeSeconds;
        double playerTime;      
        bool isPlaying = false;
        bool isRestart = false;
    };
}awdw
