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

			void DisplayDebug(float dt);
		protected:
			void GetFPSCount(float dt);
			void GetCollisionInfo();
			void GetMemoryUsage();
			void GetFeatureCosts();
			void GetCameraInfo();

			Camera* currentCam;
		};
	}
}