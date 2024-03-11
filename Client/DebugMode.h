#pragma once
#include "Vector2.h"
#include "Debug.h"
#include "GameplayState.h"


namespace NCL {
	namespace CSC8503 {
		class DebugMode
		{
		public:

			DebugMode(Camera* cam)
			{
				currentCam = cam;
			}

			void UpdateDebugMode(float dt);
		protected:
			void DisplayFPSCount(float dt);
			void DisplayCollisionInfo();
			void DisplayMemoryUsage();
			void DisplayFeatureCosts();
			void DisplayCameraInfo();

			Camera* currentCam;
		};
	}
}