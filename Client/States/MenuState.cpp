#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Canvas* pCanvas) : State()
{
    renderer = pRenderer;
    world = pGameworld;
    // Please do not change this, has to be thread safe.
    baseClient = pClient;
    canvas = pCanvas;
}

void MenuState::DogeEnter(Element& element) {
    element.SetColor({1.0, 0.5, 0.2, 1.0});
}

void MenuState::DogeExit(Element& element) {
    element.SetColor({1.0, 1.0, 1.0, 1.0});
}

void MenuState::DogeClick(Element& element) {
    canvas->PushActiveLayer("pause");
}

void MenuState::GreenBlobClick(Element& element) {
    canvas->PopActiveLayer();
}

void MenuState::InitCanvas() {
    auto& element = canvas->AddImageElement("Default.png")
            .SetAbsoluteSize({512, 512})
            .AlignMiddle()
            .AlignCenter();

    element.OnMouseEnter.connect<&MenuState::DogeEnter>(this);
    element.OnMouseExit.connect<&MenuState::DogeExit>(this);
    element.OnMouseDown.connect<&MenuState::DogeClick>(this);

    canvas->CreateNewLayer("pause");

    auto& pauseElement = canvas->AddElement("pause")
            .SetColor({0.5, 1.0, 0.2, 1.0})
            .SetAbsoluteSize({100, 100})
            .AlignTop(10)
            .AlignRight(10);

    pauseElement.OnMouseUp.connect<&MenuState::GreenBlobClick>(this);
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

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::L) && connectState == 0) {
        ConnectToGame("127.0.0.1");
    }

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R) && connectState == 2) {
        StartGame();
    }

    Debug::Print(statusText, Vector2(10, 20));
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
