#pragma once
#include <functional>
#include "NetworkBase.h"
#include "entt.hpp"

namespace NCL {
    namespace CSC8503 {
        class GameWorld;

        typedef _ENetPeer ENetPeer;
        typedef entt::sigh<void(int)> PlayerSignalH;

        struct PlayerInfo {
            int peerId;
        };

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

            void AddPlayerInfo(const PlayerInfo& info);

            virtual void UpdateServer();

            int currentSnapshot;

            entt::sink<PlayerSignalH> OnPlayerJoined {playerJoined};
            entt::sink<PlayerSignalH> OnPlayerLeft {playerLeft};

        protected:
            int			port;
            int			clientMax;
            int			clientCount;
            GameWorld*	gameWorld;

            int incomingDataRate;
            int outgoingDataRate;

            std::vector<PlayerInfo> players;

            std::map<int, int> lastPlayerUpdate;
            std::unordered_map<int, ENetPeer*> idToPeer;

            PlayerSignalH playerJoined;
            PlayerSignalH playerLeft;

        };
    }
}