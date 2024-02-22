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

            void SendInputData();
            void CreatePlayers();

            void FinishLoading();

            static void ThreadUpdate(GameClient *client, ClientNetworkData *networkData);

            void ReadNetworkFunctions();

            void ReadNetworkPackets();

        };
    }
}