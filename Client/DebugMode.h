#pragma once
#include "TutorialGame.h"


namespace NCL {
	namespace CSC8503 {
		class DebugMode
		{


		public:
			void DisplayDebug(float dt);
		protected:
			void GetFPSCount(float dt);
			void GetCollisionInfo();
			void GetMemoryUsage();
			void GetFeatureCosts();
		};
	}
}