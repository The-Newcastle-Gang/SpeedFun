#pragma once
#include "State.h"
#include "GameServer.h"
#include "RendererBase.h"
#include "PacketTypes.h"
#include "Replicated.h"
#include <iostream>

namespace NCL {
	namespace CSC8503 {
		class WaitingPlayers : public State
		{
		public:
			WaitingPlayers(GameServer* pServerBase);
			~WaitingPlayers();
			void Update(float dt);

			void OnEnter() override;
			void OnExit() override;
            void ReceivePacket(int type, GamePacket* payload, int source) override;
            bool CheckPlayersReady();

		protected:
            GameServer* serverBase;
            bool isReady;
            void StartGame();
            void AddPlayer(int peerId);
        };
	}
}