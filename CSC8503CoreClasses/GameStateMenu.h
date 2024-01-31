#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class GameStateMenu: public State
		{
		public:
			GameStateMenu();
			~GameStateMenu();
			void Update(float dt);
		protected:

		};
	}
}
