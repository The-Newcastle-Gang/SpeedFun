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

#include <iostream>

namespace NCL {
    namespace CSC8503 {
        class GameplayState : public State
        {
        public:
            GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient);
            ~GameplayState();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;

            bool IsDisconnected();
            void ReceivePacket(int type, GamePacket* payload, int source) override;

        protected:
            void InitialiseAssets();
            void InitCamera();
            void InitWorld();
            void AssignPlayer(int netObject);


#ifdef USEVULKAN
            GameTechVulkanRenderer* renderer;
#else
            GameTechRenderer* renderer;
#endif
            GameWorld* world;
            GameClient* baseClient;
            std::unique_ptr<Resources> resources;
            std::unique_ptr<Replicated> replicated;

            Transform* firstPersonPosition;

            Diagnostics packetsSent{};

            void SendInputData();
            void CreatePlayers();

            void FinishLoading();
        };
    }
}