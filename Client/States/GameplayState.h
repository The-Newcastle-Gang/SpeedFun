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
#include "SoundManager.h"


#include <thread>
#include <iostream>

namespace NCL {
    namespace CSC8503 {

        enum LoadingStates {
            NOT_LOADED,
            LOADED,
            READY
        };

        class GameplayState : public State
        {
        public:
            GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Resources* pResources, Canvas* pCanvas, SoundManager* pSoundManager);
            ~GameplayState();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;

            bool IsDisconnected();

        protected:
            void InitialiseAssets();
            void InitCamera();
            void InitWorld();
            void InitSounds();
            void AssignPlayer(int netObject);
            void CreateNetworkThread();

            void InitLevel();
            void InitCanvas();

            void ResetCameraToForwards();

            void InitCrossHeir();
            void InitTimerBar();
            void InitLevelMap();


            void SetTestSprings();
            void SetTestFloor();


#ifdef USEVULKAN
            GameTechVulkanRenderer* renderer;
#else
            GameTechRenderer* renderer;
#endif
            SoundManager* soundManager;
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

            

            void SendInputData();
            void CreatePlayers();

            void ManageLoading(float dt);
            void FinishLoading();
            std::thread* loadWorldThread;
            std::thread* loadSoundThread;
            LoadingStates soundHasLoaded = LoadingStates::NOT_LOADED;
            LoadingStates worldHasLoaded = LoadingStates::NOT_LOADED;
            LoadingStates finishedLoading = LoadingStates::NOT_LOADED;
            float loadingTime = 0.0f;

            static void ThreadUpdate(GameClient *client, ClientNetworkData *networkData);

            void ReadNetworkFunctions();


            void ReadNetworkPackets();

            void CreateRock();
            void ResetCameraAnimation();


            void WalkCamera(float dt);
            float groundedMovementSpeed = 0.0f;
            float currentGroundSpeed = 0.0f;
            float walkTimer = 0.0f;
            float maxMoveSpeed = 15.0f;
            const float bobAmount = 0.1f;
            const float bobFloor = -0.015f;
            float walkSoundTimer = 0.0f;

            Vector3 playerVelocity;

            bool isGrounded = false;
            bool isGrappling = false;

            void JumpCamera(float dt);
            float jumpTimer = 0.0f;
            const float jumpBobAmount = 0.55f;
            const float jumpAnimationSpeed = 18.0f;

            void LandCamera(float dt);
            float landTimer = 0.0f;
            const float landFallMax = 10.0f;
            float landIntensity = 0.0f;
            const float landBobAmount = 0.2f;
            const float landAnimationSpeed = 15.0f;

            void StrafeCamera(float dt);
            float strafeSpeed = 0.0f;
            float strafeAmount = 0.0f;
            const float strafeSpeedMax = 12.0f;
            float strafeTiltAmount = 1.0f;

            float defaultFOV = 40.0f;

            void HandleGrappleEvent(int event);

            float levelLen;
            Vector3 startPos;
            float CalculateCompletion(Vector3 cp);
            Element* timeBar;
            int PlayerBlip;

            void UpdatePlayerBlip(Element &element, float dt);
        };
    }
}