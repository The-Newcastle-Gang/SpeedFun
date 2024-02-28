#pragma once
#include "State.h"
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "GameClient.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "Resources.h"
#include "ClientNetworkData.h"
#include "ClientThread.h"
#include "LevelBuilder.h"
#include "InputListener.h"
#include "TriggerVolumeObject.h"

#include <thread>
#include <iostream>

namespace NCL {
    namespace CSC8503 {
        class GameplayState : public State
        {
        public:
            GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Resources* pResources, Canvas* pCanvas);
            ~GameplayState();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;

            bool IsDisconnected();

        protected:
            void InitialiseAssets();
            void InitCamera();
            void InitWorld();
            void AssignPlayer(int netObject);
            void CreateNetworkThread();
            void InitLevel();
            void InitCanvas();
            void SetTestSprings();
            void SetTestFloor();

#ifdef USEVULKAN
            GameTechVulkanRenderer* renderer;
#else
            GameTechRenderer* renderer;
#endif
            GameWorld* world;
            // DO NOT USE THIS POINTER or suffer a null pointer exception.
            GameClient* baseClient;
            Resources* resources;
            Canvas* canvas;
            std::unique_ptr<Replicated> replicated;
            // This one is thread safe, add to the outgoing and fetch from incoming.
            std::unique_ptr<ClientNetworkData> networkData;

            std::thread* networkThread;

            Transform* firstPersonPosition;

            Diagnostics packetsSent{};

            float playerMovement = 0.0f;

            void SendInputData();
            void CreatePlayers();

            void FinishLoading();

            static void ThreadUpdate(GameClient *client, ClientNetworkData *networkData);

            void ReadNetworkFunctions();

            void ReadNetworkPackets();

            void ResetCameraAnimation();
            void WalkCamera(float dt);
            float walkTimer = 0.0f;

            bool isGrounded = false;

            void JumpCamera(float dt);
            float jumpTimer = 0.0f;

            void LandCamera(float dt);
            float landTimer = 0.0f;
            const float landFallMax = 10.0f;
            float landIntensity = 0.0f;

            void StrafeCamera(float dt);
            float strafeSpeed = 0.0f;
            float strafeAmount = 0.0f;
            const float strafeSpeedMax = 12.0f;
            const float strafeTiltAmount = 0.5f;
            
        };
    }
}