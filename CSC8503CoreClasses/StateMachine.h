#pragma once

#include <map>
#include <vector>
#include "entt.hpp"
#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class State;
		class StateTransition;

		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine	{
		public:
			StateMachine();
			virtual ~StateMachine();

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			void Update(float dt);
			void ReceivePacket(int type, GamePacket *payload, int source);

		protected:
			State *activeState;
			std::vector<State*> allStates;
			TransitionContainer allTransitions;

            void Transition(State *destinationState);
        };
	}
}