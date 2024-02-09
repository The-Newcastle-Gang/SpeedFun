#pragma once
#include <chrono>
namespace NCL {
    class StageTimer {
    public:
        StageTimer();
        void Start(float dt);
        void Stop(float dt);
        void Continue(float dt);
        void ShowTimer(float dt);

    private:
        typedef std::chrono::time_point<std::chrono::high_resolution_clock> Timepoint;
        double startTime;
        double elapsedSeconds;
        bool isPlaying = false;
        bool isContinue = false;
    };
}
