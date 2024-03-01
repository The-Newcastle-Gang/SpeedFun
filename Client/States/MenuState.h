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
#include "lua.hpp"
#include "utils.h"
#include "SoundManager.h"

namespace NCL {
    namespace CSC8503 {
        class MenuState : public State, PacketReceiver
        {
        public:
            MenuState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef, Canvas* pCanvas, SoundManager* pSoundManager);
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
            SoundManager* soundManager;
            PhysicsSystem* physics;
            GameWorld* world;
            GameClient* baseClient;
            Canvas* canvas;
            std::unique_ptr<Font> menuFont;
            std::string statusText;
            std::unique_ptr<TweenManager> tweenManager;
            ShaderBase* hoverShader;
            ShaderBase* titleShader;
            int hoverBox;
            int mHoverBox;
            int selected;
            int mSelected;

            int activeText;
            int textLimit;

            bool isGameStarted;
            // Bad way to manage it, but we leave it for now.
            int connectState;

            static constexpr Vector4 inactiveMenuText = {0.2, 0.2, 0.2, 1.0};
            static constexpr Vector4 activeMenuText = {1.0, 1.0, 1.0, 1.0};

            lua_State* L;

            void ConnectToGame(const string &address);
            void RegisterPackets();
            void ConnectedToServer();
            void StartGame();
            void InitCanvas();
            void OptionHover(Element &element);
            void InitLua();
            void AttachSignals(Element& element, const std::unordered_set<std::string>& tags, const string &id);
            void AlignCanvasElement(Element &element);
            void BeginSingleplayer(Element &_);
            void AddCanvasElement(const string &layerName, bool blocking = false);
            void ShowMultiplayerOptions(Element &_);
            void MultiplayerOptionHover(Element &element);
            void JoinGame(Element &_);
            void SetActiveTextEntry(Element& element);
            void UnsetActiveTextEntry(Element& element);
            void TextEntry();
            void ConnectWithIp(Element& element);

            void InitMenuSounds();
        };
    }
}