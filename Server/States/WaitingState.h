#pragma once
#include "State.h"
#include "GameServer.h"
#include "RendererBase.h"
#include "PacketTypes.h"
#include "Replicated.h"
#include <iostream>

namespace NCL {
    namespace CSC8503 {
        class WaitingPlayers : public State, PacketReceiver
        {
        public:
            WaitingPlayers(GameServer* pServerBase);
            ~WaitingPlayers();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;
            void ReceivePacket(int type, GamePacket* payload, int source) override;
            bool CheckPlayersReady();

            int GetSelectedLevel() { return selectedLevel; }

        protected:
            int selectedLevel = 0;
            GameServer* serverBase;
            bool isReady;
            void StartGame();
            void AddPlayer(int peerId);

            void RegisterPackets();
        };
    }
}