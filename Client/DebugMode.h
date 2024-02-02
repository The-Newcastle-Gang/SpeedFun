#pragma once
#include "TutorialGame.h"


namespace NCL {
	namespace CSC8503 {
		class DebugMode
		{


		public:
			void DisplayDebug();
		protected:
			void GetFPSCount();
			void GetCollisionInfo();
			void GetMemoryUsage();
			void GetFeatureCosts();
		};
	}
}