#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class Inactive: public State
		{
		public:
			Inactive();
			~Inactive();
			void Update(float dt);
		protected:

		};
	}
}
