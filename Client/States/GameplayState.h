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
#include "InputListener.h"
#include "TriggerVolumeObject.h"
#include "DebugMode.h"
#include "SoundManager.h"
#include "AnimatorObject.h"
#include "LevelManager.h"

#include <thread>
#include <iostream>


namespace NCL {
    namespace CSC8503 {
        class DebugMode;

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

            bool hasReachedEnd = false;
            void InitEndScreen(Vector4 color);

            void SetTestSprings();
            void AddPointLight(PointLightInfo light);
            void SetTestFloor();

            std::unique_ptr<LevelManager> levelManager;

            std::string medalImage;
            std::string crosshairImage;
            std::string playerblipImage;

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

            std::atomic<bool> shouldShutDown;

            Transform* firstPersonPosition;

            Diagnostics packetsSent{};

            TextureBase* deathImageTex;


            constexpr static float chainSize = 0.1f;
            constexpr static int chainLinkCount = (int)(Replicated::GRAPPLEDISTANCE / chainSize / 0.5); // Grapple distance / chainLength(-1 -> 1 * 0.2) / 0.9


            GameObject* chains[chainLinkCount * Replicated::PLAYERCOUNT];
            GameObject* grapples[Replicated::PLAYERCOUNT];

            void SendInputData();
            void CreatePlayers();

            void UpdatePlayerAnimation(int networkID, Replicated::PlayerAnimationStates state);

            void ManageLoading(float dt);
            void FinishLoading();
            std::thread* loadWorldThread;
            std::thread* loadSoundThread;
            LoadingStates soundHasLoaded = LoadingStates::NOT_LOADED;
            LoadingStates worldHasLoaded = LoadingStates::NOT_LOADED;
            LoadingStates finishedLoading = LoadingStates::NOT_LOADED;
            bool isSinglePlayer;
            float loadingTime = 0.0f;

            float totalDTElapsed = 0.0f;
            bool debugMovementEnabled = false;

            void ThreadUpdate(GameClient *client, ClientNetworkData *networkData);
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

            float defaultFOV = 70.0f;

            void HandleGrappleEvent(int event);

            float levelLen;
            Vector3 startPos;
            Vector3 endPos;

            Vector3 deathPos;
            float CalculateCompletion(Vector3 cp);
            void AddLava( Vector3 pos );
            void AddEndPortal ( Vector3 pos );
            int PlayerBlip;

            float timeElapsed;
            int currentMedal = 4;
            float timerMedalShakeTimer = 0.0f;
            float medalTimes[3] = { -1.0f, -1.0f, -1.0f };

            void InitPlayerBlip(int id);
            std::unordered_map<std::string, Vector3> playerPositions;

            std::unordered_map<std::string, std::pair<int, float>> medalTimeRatios;

            Element *timerNubs[3];
            Element *timeBar;
            Element *timeBarTimerBoxBack;
            Element *timeBarTimerBox;
            Element *timerText;
            ShaderBase *timerBarShader;
            ShaderBase *fireShader;

            const int timerTopOffset = 32;
            const int timerBarHeight = 10;
            const int timerBoxWidth = 10;

            const int timerEndBoxY = 60;
            const int timerEndBoxX = 60;
            const float timerEndBoxYoff = 0.6f;

            const int timerBarOutline = 3;
            float timerRatio = 0.0f;
            Vector4 timerBarColor = { Replicated::PLATINUM };

            void InitialiseMedalNubs();
            void UpdateTimerUI(Element& element, float dt);
            void UpdateTimerBox(Element& element, float dt);
            void UpdateTimerText(Element& element, float dt);
            void UpdateTimerNub(Element& element, float dt);
            void UpdatePlayerBlip(Element &element, float dt);

            Element* crosshair;
            void UpdateCrosshair(Element& element, float dt);
            float crossHairRotation = 45.0f;
            float currentCHRotation = 0.0f;
            int rotationDirection = 1.0f;
            float crossHairScale = 1.0f;

            enum MedalAnimationStages {
                START,
                TIMER_SCROLL,
                TIMER_SHAKE,
                MEDAL
            };

            ShaderBase *medalShineShader;

            std::string GetMedalImage();
            void UpdateMedalSprite(Element& element, float dt);
            void UpdateFinalTimeTally(Element& element, float dt);
            float medalTimer = 0.0f;
            float finalTime = 0.0f;
            float finalTimeScroll = 0.0f;
            float finaltimeShrink = 1.0f;
            float finalTimeShake = 0.0f;
            MedalAnimationStages medalAnimationStage = MedalAnimationStages::START;
            std::unique_ptr<Font> biggerDebugFont;
            DebugMode* debugger;
            bool displayDebugger = false;

            void RenderFlairObjects();
            void CreateGrapples();
            void UpdateGrapples();
            GameObject *CreateChainLink();
            void CreateChains();
            void OperateOnChains(int grappleIndex, const std::function<void(GameObject &, int)>& opFunction);
            void OnGrappleToggle(GameObject &gameObject, bool isActive);
        };
    }
}