#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Canvas* pCanvas) : State()
{
    renderer = pRenderer;
    world = pGameworld;
    menuFont = renderer->LoadFont("IndigoRegular.otf");
    baseClient = pClient;
    tweenManager = std::make_unique<TweenManager>();
    canvas = pCanvas;
}

void MenuState::OptionHover(Element& element) {

    // Had to switch this to return a reference? Hope there's a better way, feels wrong.
    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementById(hoverBox);
    tweenManager->CreateTween(
            TweenManager::EaseOutElastic,
            boxElement.GetAbsolutePosition().y,
            pos - 33,
            &boxElement.GetAbsolutePosition().y,
            0.5f);
    boxElement.AlignLeft(95);

}

void MenuState::InitCanvas() {
    // This is killing me inside we need to load this in somehow.
    canvas->AddElement()
            .SetColor({1.0f, 244.0f/255.0f, 1.0f, 0.8f})
            .SetRelativeSize({0.0f, 1.0f})
            .SetAbsoluteSize({415, 0})
            .AlignLeft(115);

    canvas->AddElement()
           .SetColor({150.0f/255.0f, 0.0f, 210.0f/255.0f, 1.0f})
           .SetAbsoluteSize({550, 220})
           .AlignTop()
           .AlignLeft(50);

    canvas->AddImageElement("Menu/TitleSpeed.png")
            .SetAbsoluteSize({290, 97})
            .SetAbsolutePosition({280, 0})
            .AlignTop(25);

    canvas->AddImageElement("Menu/TitleFun.png")
            .SetAbsoluteSize({151, 77})
            .SetAbsolutePosition({407, 0})
            .AlignTop(122);

    canvas->AddImageElement("Menu/Dashes.png")
            .SetAbsoluteSize({109, 70})
            .SetAbsolutePosition({285, 0})
            .AlignTop(128);

    hoverBox = canvas->AddElement()
            .SetAbsoluteSize({460, 115})
            .SetColor({160.0f/255.0f, 20.0f / 255.0f, 220.0f/255.0f, 1.0f})
            .AlignTop(247)
            .AlignLeft(95)
            .GetIndex();

    auto& singleplayer = canvas->AddImageElement("Menu/Singleplayer.png")
            .SetAbsoluteSize({315, 54})
            .SetAbsolutePosition({160, 0})
            .AlignTop(280);

    singleplayer.OnMouseEnter.connect<&MenuState::OptionHover>(this);

    auto& multiplayer = canvas->AddImageElement("Menu/Multiplayer.png")
            .SetAbsoluteSize({290, 54})
            .SetAbsolutePosition({160, 0})
            .AlignTop(390)
            .ExtendUpperBounds(20, 0);

    multiplayer.OnMouseEnter.connect<&MenuState::OptionHover>(this);

    auto& exit = canvas->AddImageElement("Menu/Exit.png")
            .SetAbsoluteSize({97, 43})
            .SetAbsolutePosition({160, 0})
            .AlignTop(610)
            .ExtendUpperBounds(215, 0);

    exit.OnMouseEnter.connect<&MenuState::OptionHover>(this);

    auto& options = canvas->AddImageElement("Menu/Options.png")
            .SetAbsoluteSize({201, 55})
            .SetAbsolutePosition({160, 0})
            .AlignTop(500)
            .ExtendUpperBounds(115, 0);

    options.OnMouseEnter.connect<&MenuState::OptionHover>(this);
}

void MenuState::OnEnter() {
    isGameStarted = false;
    statusText = "Press L to connect to localhost";
    connectState = 0;

    InitCanvas();

    RegisterPackets();
}

void MenuState::RegisterPackets() {
    baseClient->RegisterPacketHandler(Function, this);
}

bool MenuState::CheckConnected() const {
    return isGameStarted;
}

void MenuState::ConnectedToServer() {
    statusText = "Connected! Press R to start";
    connectState = 2;
}

void MenuState::ConnectToGame(const std::string& address) {
    baseClient->OnServerConnected.connect<&MenuState::ConnectedToServer>(this);
    baseClient->Connect(address, NetworkBase::GetDefaultPort());
    statusText = "Attempting to connect to localhost";
    connectState = 1;
}

void MenuState::StartGame() {
    baseClient->RemoteFunction(Replicated::StartGame, nullptr);
}

void MenuState::Update(float dt) {

    baseClient->UpdateClient();

    tweenManager->Update(dt);

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::L) && connectState == 0) {
        ConnectToGame("127.0.0.1");
    }

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R) && connectState == 2) {
        StartGame();
    }

    //Debug::Print(statusText, Vector2(10, 20));
    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void MenuState::ReceivePacket(int type, GamePacket *payload, int source) {
    switch(type) {
        case Function: {
            auto packet = reinterpret_cast<FunctionPacket*>(payload);
            if (packet->functionId == Replicated::RemoteClientCalls::LoadGame) {
                isGameStarted = true;
            }

        } break;
    }
}

void MenuState::OnExit() {
    baseClient->OnServerConnected.disconnect(this);
    baseClient->ClearPacketHandlers();
    canvas->Reset();
}

MenuState::~MenuState() {

}
