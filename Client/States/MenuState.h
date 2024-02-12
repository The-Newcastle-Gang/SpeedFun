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
#include "TweenManager.h"

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
            std::unique_ptr<Font> menuFont;
            std::string statusText;
            std::unique_ptr<TweenManager> tweenManager;
            int hoverBox;

            bool isGameStarted;
            // Bad way to manage it, but we leave it for now.
            int connectState;


            void ConnectToGame(const string &address);
            void RegisterPackets();
            void ConnectedToServer();
            void StartGame();

            void InitCanvas();

            void OptionHover(Element &element);
        };
    }
}