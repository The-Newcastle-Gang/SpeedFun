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
            typedef entt::sigh<void()> ConnectionH;
        public:
            GameClient();
            ~GameClient();

            bool Connect(const std::string& ip, int portNum);
            void Disconnect();

            void SendPacket(GamePacket& payload);
            void SendImportantPacket(GamePacket &payload);

            void UpdateClient();

            int lastServerSnapshot;

            entt::sink<ConnectionH> OnServerConnected;
            void RemoteFunction(int functionId, FunctionData *data);
            Diagnostics packetsReceived{};
            Diagnostics packetsSent{};
            Diagnostics updateCalled{};

            void PlayLoadingScreen();

        protected:
            _ENetPeer*	netPeer;
            ConnectionH serverConnected;

            void UpdateDiagnostics(Diagnostics &d);


        };
    }
}