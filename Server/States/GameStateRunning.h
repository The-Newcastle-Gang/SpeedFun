#pragma once
#include "State.h"
#include <iostream>
namespace NCL {
	namespace CSC8503 {
		class Running : public State
		{
		public:
			Running();
			~Running();
			void Update(float dt);

			void OnEnter() { std::cout << "On Enter"; }
			void OnExit() { std::cout << "On Exit"; }
		protected:

		};
	}
}