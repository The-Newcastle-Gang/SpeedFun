#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class Loading : public State
		{
		public:
			Loading();
			~Loading();
			void Update(float dt);
		protected:

		};
	}
}
