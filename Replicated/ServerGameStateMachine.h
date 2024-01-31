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
		class ServerGameStateMachine : public StateMachine {
		public:
			ServerGameStateMachine();
			virtual ~ServerGameStateMachine();
			void SetAvailableLevels(std::vector<State*> levels)
			{
				availableLevels = levels;
			}
			void SetServerState(ServerStates serverState)
			{
				currentServerState = serverState;
			}
		protected:
			ServerStates currentServerState;
			std::vector<State*> availableLevels;
			void InitialiseServerStateMachine();
		};
	}
}