#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class GameStateLoading : public State
		{
		public:
			GameStateLoading();
			~GameStateLoading();
			void Update(float dt);
		protected:

		};
	}
}
