#pragma once
#include "State.h"
#include <iostream>

namespace NCL {
	namespace CSC8503 {
		class Busy : public State
		{
		public:
			Busy();
			~Busy();
			void Update(float dt);

			void OnEnter() {}
			void OnExit() {}
		protected:

		};
	}
}