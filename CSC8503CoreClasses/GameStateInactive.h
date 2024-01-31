#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class GameStateInactive: public State
		{
		public:
			GameStateInactive();
			~GameStateInactive();
			void Update(float dt);
		protected:

		};
	}
}
