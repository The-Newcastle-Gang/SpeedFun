#pragma once
#include <functional>
#include "NetworkBase.h"
#include "entt.hpp"

namespace NCL {
    namespace CSC8503 {
        class GameWorld;
        typedef _ENetPeer ENetPeer;
        typedef entt::sigh<void(int)> PlayerSignalH;
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

            entt::sink<PlayerSignalH> OnPlayerJoined {playerJoined};
            entt::sink<PlayerSignalH> OnPlayerLeft {playerLeft};

        protected:
            int			port;
            int			clientMax;
            int			clientCount;
            GameWorld*	gameWorld;

            int incomingDataRate;
            int outgoingDataRate;

            std::map<int, int> lastPlayerUpdate;
            std::unordered_map<int, ENetPeer*> idToPeer;

            PlayerSignalH playerJoined;
            PlayerSignalH playerLeft;

        };
    }
}