#pragma once
#include "StateMachine.h"
#include <vector>
namespace NCL {
	namespace CSC8503 {
		enum ServerStates {
			Inactive,
			Loading,
			Menu,
			InGameplay
		};
		class ServerGameStateMachine : public StateMachine {
		public:
			ServerGameStateMachine();
			virtual ~ServerGameStateMachine();
			void SetAvailableLevels(std::vector<State*> levels)
			{
				availableLevels = levels;
			}
		protected:
			ServerStates currentServerState;
			std::vector<State*> availableLevels;
			void InitialiseServerStateMachine();
			void UpdateInactive();
			void UpdateLoading();
			void UpdateMenu();
			void UpdateInGameplay();
		};
	}
}