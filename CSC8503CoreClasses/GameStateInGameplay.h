#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class InGameplay: public State
		{
		public:
			InGameplay();
			~InGameplay();
			void Update(float dt);
		protected:

		};
	}
}
