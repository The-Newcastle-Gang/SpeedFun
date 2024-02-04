#pragma once
#include "StateMachine.h"
#include <vector>
#include "Renderer.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {

		class ClientMachine : public StateMachine {
		public:
            enum ClientStates {
                InactiveState,
                LoadingState,
                MenuState,
                InGameplayState
            };

			ClientMachine();
			~ClientMachine() override = default;
			std::vector<State*> GetAvailableLevels() {
				return availableLevels;
			}
			void SetClientState(ClientStates clientState) {
				currentClientState = clientState;
			}


		protected:


			ClientStates currentClientState;
			std::vector<State*> availableLevels;
			void InitialiseClientStateMachine();

			bool ForceTransition(bool hasConditionMet, ClientStates stateSwitchTo);

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif

			GameWorld* gameWorld;
		};
	}
}