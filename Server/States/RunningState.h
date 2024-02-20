#pragma once
#include "State.h"
#include "GameServer.h"
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "ServerNetworkData.h"
#include "ServerThread.h"
#include "PlayerMovement.h"

#include <iostream>
#include <thread>

namespace NCL {
    namespace CSC8503 {
        class RunningState : public State
        {
        public:
            RunningState(GameServer* pBaseServer);
            ~RunningState();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;

            GameObject* GetPlayerObjectFromId(int peerId) {
                return playerObjects[peerId];
            }

            int GetIdFromPlayerObject(GameObject* obj) {
                for (auto& p : playerObjects) {
                    if (p.second == obj) return p.first;
                }

                return -1;
            }

        protected:
            GameServer* serverBase;
            std::unique_ptr<Replicated> replicated;
            std::unique_ptr<PhysicsSystem> physics;
            std::unique_ptr<GameWorld> world;
            std::unique_ptr<ServerNetworkData> networkData;

            std::thread* networkThread;

            std::unordered_map<int, PlayerInfo> playerInfo;

            LevelReader* levelReader;
            Vector3 currentLevelStartPos;

            float packetTimer;
            int sceneSnapshotId;

            int playerTestId = -1;

            std::unordered_map<int, GameObject*> playerObjects;

            void LoadLevel();
            void BuildLevel(const std::string &levelName);
            void CreatePlayers();

            void SendWorldToClient();

            Diagnostics dia{};

            void Tick(float dt);
            void AssignPlayer(int peerId, GameObject *object);

            void UpdatePlayerMovement(GameObject *player, const InputPacket& inputInfo);
            static void ThreadUpdate(GameServer* server, ServerNetworkData *networkData);

            void CreateNetworkThread();

            void ReadNetworkFunctions();

            void ReadNetworkPackets();

            void ApplyPlayerMovement();
        };
    }
}