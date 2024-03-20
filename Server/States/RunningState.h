#pragma once
#include "State.h"
#include "GameServer.h"
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "ServerNetworkData.h"
#include "ServerThread.h"
#include "TriggerVolumeObject.h"
#include "PlayerMovement.h"
#include "DamagingObstacle.h"
#include "Levels/LevelManager.h"
#include "ObjectOscillator.h"
#include "PlayerRespawner.h"
#include "Swinging.h"
#include "Spring.h"
#include "TestBridge.h"
#include "BridgeTrigger.h"
#include "TrapObject.h"
#include "RayEnemyFollow.h"
#include "RayEnemyShoot.h"
#include "SpeedUpBlock.h"

#include <iostream>
#include <thread>

namespace NCL {
    namespace CSC8503 {

        namespace RunningStateEnums {
            enum RunningStateEnum {
                COUNTDOWN,
                GAMEPLAY,
                END_OF_LEVEL
            };
        }

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

            void SetSelectedLevel(int level) { selectedLevel = level; }

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

            std::array<GameObject *, Replicated::PLAYERCOUNT> grapples;
            std::atomic<bool> shouldClose;

            bool isPaused = false;
            RunningStateEnums::RunningStateEnum state = RunningStateEnums::COUNTDOWN;
            float packetTimer;
            int sceneSnapshotId;

            int selectedLevel = 0;
            int playerTestId = -1;

            void WaitUntilClientsInGameplay();
            int numPlayersInGameplayState = 0;

            void WaitForPlayersLoaded();
            int numPlayersLoaded = 0;

            bool shouldMoveToNewLevel = false;

            std::unordered_map<int, GameObject*> playerObjects;
            std::unordered_map<int, bool> playersFinished;
            std::unordered_map<int, float> playerTimes;

            bool hasAllPlayersFinished = false;

            void ResetLevelInfo();
            void SendLevelToClients(int level);
            void LoadLevel(int level);
            void BuildLevel(const std::string &levelName);
            void CreatePlayers();

            void MoveToNewLevel(int level);
            void ClearLevel();

            void UpdateInCountdown(float dt);
            void UpdateInGameplay(float dt);
            void UpdateInEndOfLevel(float dt);

            void StartTriggerVolFunc(int id);
            void EndTriggerVolFunc(int id);
            void OnAllPlayersFinished();
            void DeathTriggerVolFunc(int id);
            void DeathTriggerVolEndFunc(int id);

            void SendMedalValues();
            void SendWorldToClient();
            void SendMedalToClient(int id);

            Diagnostics dia{};

            void Tick(float dt);
            void AssignPlayer(int peerId, GameObject *object);
            void AddTriggersToLevel();
            void SortTriggerInfoByType(TriggerVolumeObject::TriggerType &triggerType, Vector4 &colour, Vector3 &dimensions);

            void UpdatePlayerGameInfo();
            void UpdateGameTimerInfo();
            void UpdatePlayerPositionsInfo();
            void UpdatePlayerMovement(GameObject *player, const InputPacket& inputInfo);
            void ResetAllPlayerMoveInputs();
            void ResetPlayerMoveInputs(GameObject* playerObject);
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
            void CreateGrapples();
            void SetAllGrapplesInactive();
            void SetNetworkActive(GameObject *g, bool isActive);
            void GrappleEnd(GameObject *player);
            void GrappleUpdate(GameObject *player, Vector3 position);
            void GrappleStart(GameObject *player, Vector3 direction);
            void CancelGrapple(int id);

            RayEnemyFollow* re;
            TestBridge* ib;
        };
    }
}