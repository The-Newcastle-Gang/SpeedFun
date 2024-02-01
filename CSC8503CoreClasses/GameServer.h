#pragma once
#include <functional>
#include "NetworkBase.h"
#include "entt.hpp"

namespace NCL {
    namespace CSC8503 {
        class GameWorld;
        typedef _ENetPeer ENetPeer;
        class GameServer : public NetworkBase {
        public:
            GameServer(int onPort, int maxClients);
            ~GameServer();

            bool Initialise();
            void Shutdown();

            void SetGameWorld(GameWorld &g);

            bool SendGlobalPacket(int msgID);
            bool SendGlobalPacket(GamePacket& packet);

            bool SendPacket(GamePacket& packet, int peerId);

            virtual void UpdateServer();

            int currentSnapshot;
            std::function<void(int)> connectCallback;
            std::function<void(int)> leaveCallback;

        protected:
            int			port;
            int			clientMax;
            int			clientCount;
            GameWorld*	gameWorld;

            int incomingDataRate;
            int outgoingDataRate;

            std::map<int, int> lastPlayerUpdate;
            std::unordered_map<int, ENetPeer*> idToPeer;

        };
    }
}