#pragma once
#include <functional>
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
		protected:
			StateUpdateFunction func;
		};
	}
}