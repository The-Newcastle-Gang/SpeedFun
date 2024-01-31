#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class Busy : public State
		{
		public:
			Busy();
			~Busy();
			void Update(float dt);
		protected:

		};
	}
}
