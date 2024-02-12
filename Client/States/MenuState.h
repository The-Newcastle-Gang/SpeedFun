#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "GameClient.h"
#include "Replicated.h"
#include "PacketTypes.h"
#include "Canvas.h"

namespace NCL {
    namespace CSC8503 {
        class MenuState : public State, PacketReceiver
        {
        public:
            MenuState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef, Canvas* pCanvas);
            ~MenuState();
            void Update(float dt) override;

            void OnEnter() override;
            void OnExit() override;
            void ReceivePacket(int type, GamePacket *payload, int source) override;

            bool CheckConnected() const;

        protected:
#ifdef USEVULKAN
            GameTechVulkanRenderer* renderer;
#else
            GameTechRenderer* renderer;
#endif
            PhysicsSystem* physics;
            GameWorld* world;
            GameClient* baseClient;
            Canvas* canvas;
            std::string statusText;
            Element* hoverBox;

            bool isGameStarted;
            // Bad way to manage it, but we leave it for now.
            int connectState;


            void ConnectToGame(const string &address);
            void RegisterPackets();
            void ConnectedToServer();
            void StartGame();

            void DogeEnter(Element &element);

            void DogeExit(Element &element);

            void DogeClick(Element &element);

            void GreenBlobClick(Element &element);

            void InitCanvas();

            void OptionHover(Element &element);
        };
    }
}