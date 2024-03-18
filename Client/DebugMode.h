#pragma once
#include "Vector2.h"
#include "Debug.h"
#include "GameplayState.h"
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

		protected:
			static void DisplayFPSCount(float dt);
			static void DisplayCollisionInfo();
			static void DisplayMemoryUsage();
			static void DisplayFeatureCosts();
			static void DisplayCameraInfo();


			static Camera* currentCam;

            static double GetCurrentCPUVal();
            static double OurCurrentUsage;

            static std::chrono::steady_clock::time_point start;
            static std::chrono::steady_clock::time_point end;

            static std::vector<float> timeValues;
        };
	}
}