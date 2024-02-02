#pragma once
#include "NetworkBase.h"
#include "GameWorld.h"

#include <stdint.h>
#include <thread>
#include <atomic>

namespace NCL {
    namespace CSC8503 {
        class GameObject;
        class GameClient : public NetworkBase {


        public:
            GameClient();
            ~GameClient();

            bool Connect(std::string ip, int portNum);
            void Disconnect();

            void SendPacket(GamePacket& payload);
            void SendPacket(GamePacket& payload, int something);

            void UpdateClient();

            int lastServerSnapshot;
        protected:
            _ENetPeer*	netPeer;
        };
    }
}