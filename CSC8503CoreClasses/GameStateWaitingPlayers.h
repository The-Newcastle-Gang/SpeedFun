#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class WaitingPlayers : public State
		{
		public:
			WaitingPlayers();
			~WaitingPlayers();
			void Update(float dt);
		protected:

		};
	}
}
