#pragma once

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

			virtual void OnEnter() { std::cout << "On Enter"; }
			virtual void OnExit() { std::cout << "On Exit"; }
		protected:
			StateUpdateFunction func;
		};
	}
}