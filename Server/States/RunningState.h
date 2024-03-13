#pragma once
#include "State.h"
#include "GameServer.h"
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "ServerNetworkData.h"
#include "ServerThread.h"
#include "TriggerVolumeObject.h"
#include "PlayerMovement.h"
#include "Components/SwingingObject.h"
#include "DamagingObstacle.h"
#include "Levels/LevelManager.h"
#include "ObjectOscillator.h"
#include "Spring.h"

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
            std::unique_ptr<LevelManager> levelManager;

            std::thread* networkThread;

            std::unordered_map<int, PlayerInfo> playerInfo;
            std::unordered_map<int, Replicated::PlayerAnimationStates> playerAnimationInfo;

            std::vector<std::pair<TriggerVolumeObject::TriggerType, Vector3>> triggersVector;
            LevelReader* levelReader;
            Vector3 currentLevelStartPos;
            Vector3 currentLevelEndPos;
            Vector3 currentLevelDeathPos;
            std::vector<Vector3> currentLevelCheckPointPositions;

            std::atomic<bool> shouldClose;

            float packetTimer;
            int sceneSnapshotId;

            int playerTestId = -1;

            int numPlayersLoaded = 0;
            std::unordered_map<int, GameObject*> playerObjects;

            void LoadLevel();
            void BuildLevel(const std::string &levelName);
            void CreatePlayers();

            void StartTriggerVolFunc(int id);
            void EndTriggerVolFunc(int id);
            void DeathTriggerVolFunc(int id);
            void DeathTriggerVolEndFunc(int id);

            void SendWorldToClient();

            Diagnostics dia{};

            void Tick(float dt);
            void AssignPlayer(int peerId, GameObject *object);
            void AddTriggersToLevel();
            void SortTriggerInfoByType(TriggerVolumeObject::TriggerType &triggerType, Vector4 &colour, Vector3 &dimensions);

            void UpdatePlayerMovement(GameObject *player, const InputPacket& inputInfo);
            void ThreadUpdate(GameServer* server, ServerNetworkData *networkData);

            void CreateNetworkThread();

            void ReadNetworkFunctions();

            void ReadNetworkPackets();

            void ApplyPlayerMovement();

            void UpdatePlayerAnimations();

            void SetPlayerAnimation(Replicated::PlayerAnimationStates state, GameObject* object);

            void SendPlayerAnimationCall(Replicated::PlayerAnimationStates state, GameObject* object);

            void SetTestSprings();
            void SetTestFloor();

            void SetTriggerTypePositions();

        };
    }
}