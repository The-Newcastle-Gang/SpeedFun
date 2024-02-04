#pragma once
#include "StateMachine.h"
#include <vector>
namespace NCL {
	namespace CSC8503 {
		class ServerMachine : public StateMachine {
		public:

            enum ServerStates {
                Busy,
                Waiting,
                Running
            };

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
			void InitMachine();
		};
	}
}