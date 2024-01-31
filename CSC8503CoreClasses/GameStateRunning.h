#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class Running : public State
		{
		public:
			Running();
			~Running();
			void Update(float dt);
		protected:

		};
	}
}
