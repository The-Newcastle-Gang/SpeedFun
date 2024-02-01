#pragma once
#include "State.h"
#include <iostream>

namespace NCL {
	namespace CSC8503 {
		class Menu: public State
		{
		public:
			Menu();
			~Menu();
			void Update(float dt);

			void OnEnter() { std::cout << "On Enter"; }
			void OnExit() { std::cout << "On Exit"; }
		protected:

		};
	}
}