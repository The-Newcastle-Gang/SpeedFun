#pragma once

#include "StateMachine.h"
#include "StateTransition.h"


namespace NCL {
	namespace CSC8503 {
		typedef std::function<void(float)> StateUpdateFunction;

		class  State		{
		public:
			State() {}
			State(StateUpdateFunction someFunc) {
				func		= someFunc;
			}
			
			virtual void Update(float dt) = 0;

			virtual void OnEnter() {  }
			virtual void OnExit() {  }
		protected:
			StateUpdateFunction func;
		};
	}
}