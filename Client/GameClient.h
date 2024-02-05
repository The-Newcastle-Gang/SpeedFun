#pragma once
#include "NetworkBase.h"
#include "GameWorld.h"
#include "Replicated.h"
#include "PacketTypes.h"

#include <stdint.h>
#include <thread>
#include <atomic>
#include "entt.hpp"

namespace NCL {
    namespace CSC8503 {
        class GameObject;
        class GameClient : public NetworkBase{
            typedef entt::sigh<void()> ConnectionH;;
        public:
            GameClient();
            ~GameClient();

            bool Connect(const std::string& ip, int portNum);
            void Disconnect();

            void SendPacket(GamePacket& payload);
            void SendPacket(GamePacket& payload, int something);

            void UpdateClient();

            int lastServerSnapshot;

            entt::sink<ConnectionH> OnServerConnected;
            void RemoteFunction(int functionId, FunctionData *data);

        protected:
            _ENetPeer*	netPeer;
            ConnectionH serverConnected;
        };
    }
}