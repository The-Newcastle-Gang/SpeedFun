#pragma once
#include "State.h"
#include <iostream>
#include "GameServer.h"

namespace NCL {
	namespace CSC8503 {
		class Running : public State
		{
		public:
			Running(GameServer* pBaseServer);
			~Running();
			void Update(float dt);

			void OnEnter() { std::cout << "Game Running" << std::endl; }
			void OnExit() {}
		protected:
            GameServer* baseServer;
		};
	}
}