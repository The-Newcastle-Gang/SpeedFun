#pragma once
#include "Vector2.h"
#include "Debug.h"
#include "GameplayState.h"
#include "windows.h"
#include "TCHAR.h"
#include "pdh.h"


namespace NCL {
	namespace CSC8503 {
		class DebugMode
		{
		public:
            static void SetDebugCam(Camera* cam);
			static void UpdateDebugMode(float dt);
            static void InitDebugInfo();
		protected:
			static void DisplayFPSCount(float dt);
			static void DisplayCollisionInfo();
			static void DisplayMemoryUsage();
			static void DisplayFeatureCosts();
			static void DisplayCameraInfo();

			static Camera* currentCam;

            static double GetCurrentCPUVal();
            static double OurCurrentUsage;
        };
	}
}