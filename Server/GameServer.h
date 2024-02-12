#pragma once

#include <functional>
#include <utility>
#include "NetworkBase.h"
#include "entt.hpp"
#include "GameWorld.h"
#include "Replicated.h"
#include "PacketTypes.h"

namespace NCL {
    namespace CSC8503 {
        class GameWorld;

        typedef _ENetPeer ENetPeer;
        typedef entt::sigh<void(int)> PlayerSignalH;

        // Put any player info needed by multiple states here.
        // This can be extracted into it's own class at some point.
        struct PlayerInfo {
            int peerId;
        };

        class GameServer : public NetworkBase {
        public:
            GameServer(int onPort, int maxClients);
            ~GameServer();

            bool Initialise();
            void Shutdown();

            bool SendGlobalPacket(int msgID);
            bool SendGlobalPacket(GamePacket& packet);
            bool SendGlobalImportantPacket(GamePacket& packet);

            bool SendPacket(GamePacket& packet, int peerId);
            bool SendImportantPacket(GamePacket& packet, int peerId);

            void AddPlayerInfo(int peerId, PlayerInfo& info);
            std::unordered_map<int, PlayerInfo>& GetPlayerInfo();
            PlayerInfo GetPlayerByPeerId(int peerId);


            virtual void UpdateServer();

            void CallRemote(int functionId, FunctionData *data, int peer);
            void CallRemoteAll(int functionId, FunctionData *data);

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

            std::unordered_map<int, PlayerInfo> players;

            std::map<int, int> lastPlayerUpdate;
            std::unordered_map<int, ENetPeer*> idToPeer;

            PlayerSignalH playerJoined;
            PlayerSignalH playerLeft;

            Diagnostics packetsSent{};
            Diagnostics packetsRecieved{};

            void RemovePlayerInfo(int peerId);
            bool UpdateDiagnostics(Diagnostics &d);
        };
    }
}