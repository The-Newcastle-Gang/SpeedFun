#pragma once
#include "TutorialGame.h"


namespace NCL {
	namespace CSC8503 {
		class DebugMode
		{


		public:
			void DisplayDebug(float dt);
		protected:
			void DisplayFPSCount(float dt);
			void DisplayCollisionInfo();
			void DisplayMemoryUsage();
			void DisplayFeatureCosts();

		};
	}
}