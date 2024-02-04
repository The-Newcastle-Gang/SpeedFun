#pragma once
#include "StateMachine.h"
#include <vector>
namespace NCL {
	namespace CSC8503 {
		enum ServerStates {
			BusyState,
			WaitingPlayersState,
			RunningState
		};
		class ServerMachine : public StateMachine {
		public:
			ServerMachine();
			~ServerMachine() override = default;
            // Assuming this is the only place these levels are stored use rvalue reference.
			void SetAvailableLevels(std::vector<State*>&& levels)
			{
				availableLevels = levels;
			}
		protected:
			ServerStates currentServerState;
			std::vector<State*> availableLevels;
			void InitialiseServerStateMachine();
		};
	}
}