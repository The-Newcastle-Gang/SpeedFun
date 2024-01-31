#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class GameStateInGameplay: public State
		{
		public:
			GameStateInGameplay();
			~GameStateInGameplay();
			void Update(float dt);
		protected:

		};
	}
}
