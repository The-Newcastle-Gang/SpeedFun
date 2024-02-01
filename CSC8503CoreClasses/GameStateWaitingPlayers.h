#pragma once
#include "State.h"
#include <iostream>

namespace NCL {
	namespace CSC8503 {
		class WaitingPlayers : public State
		{
		public:
			WaitingPlayers();
			~WaitingPlayers();
			void Update(float dt);

			void OnEnter() { std::cout << "On Enter"; }
			void OnExit() { std::cout << "On Exit"; }
		protected:

		};
	}
}
