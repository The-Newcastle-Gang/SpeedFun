#pragma once
#include "StateMachine.h"
#include <vector>
namespace NCL {
	namespace CSC8503 {
		enum ClientStates {
			Inactive,
			Loading,
			Menu,
			InGameplay
		};
		class ClientGameStateMachine : public StateMachine {
		public:
			ClientGameStateMachine();
			virtual ~ClientGameStateMachine();
			std::vector<State*> GetAvailableLevels()
			{
				return availableLevels;
			}
			void SetClientState(ClientStates clientState)
			{
				currentClientState = clientState;
			}
		protected:
			ClientStates currentClientState;
			std::vector<State*> availableLevels;
			void InitialiseClientStateMachine();
			void UpdateInactive(float dt);
			void UpdateLoading(float dt);
			void UpdateMenu(float dt);
			void UpdateInGameplay(float dt);
		};
	}
}