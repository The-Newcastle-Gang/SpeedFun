#pragma once
#include "Vector2.h"
#include "Debug.h"
#include "Camera.h"
#include "windows.h"
#include "TCHAR.h"
#include "pdh.h"
#include <chrono>



namespace NCL {
    namespace CSC8503 {
        class DebugMode
        {
        public:
            static void SetDebugCam(Camera* cam);
            static void UpdateDebugMode(float dt);
            static void InitDebugInfo();

            static void StartCostClock();
            static void EndCostClock(int idx);


            static void SetCollisionCount(int colCount) {
                collisionCount = colCount;
            }



        protected:
            static void DisplayFPSCount(float dt);
            static void DisplayCollisionInfo();
            static void DisplayMemoryUsage();
            static void DisplayFeatureCosts();
            static void DisplayCameraInfo();


            static Camera* currentCam;

            static double GetCurrentCPUVal();
            static double OurCurrentUsage;
            static int collisionCount;


            static std::chrono::steady_clock::time_point start;
            static std::chrono::steady_clock::time_point end;

            static std::vector<float> timeValues;

            static std::vector<Vector2> statPositions;
        };
    }
}