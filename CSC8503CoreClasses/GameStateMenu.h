#pragma once
#include "State.h"
namespace NCL {
	namespace CSC8503 {
		class Menu: public State
		{
		public:
			Menu();
			~Menu();
			void Update(float dt);
		protected:

		};
	}
}
