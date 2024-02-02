#pragma once
#include "StateMachine.h"
#include <vector>
#include "Renderer.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		enum ClientStates {
			InactiveState,
			LoadingState,
			MenuState,
			InGameplayState
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

			bool ForceTransition(bool hasConidtionMet, ClientStates stateSwitchTo);

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif

			GameWorld* gameWorld;
		};
	}
}