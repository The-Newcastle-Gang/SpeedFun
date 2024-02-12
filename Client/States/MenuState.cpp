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

void MenuState::OptionHover(Element& element) {
    auto pos = element.GetAbsolutePosition().y;
    hoverBox->SetAbsolutePosition({0, pos + 33});
    hoverBox->AlignLeft(95);
    std::cout << "Working?" << std::endl;
}

void MenuState::InitCanvas() {

    auto& menuBox = canvas->AddElement()
            .SetColor({1.0f, 244.0f/255.0f, 1.0f, 0.8f})
            .SetRelativeSize({0.0f, 1.0f})
            .SetAbsoluteSize({415, 0})
            .AlignLeft(115);

    auto& purpleBox = canvas->AddElement()
           .SetColor({150.0f/255.0f, 0.0f, 210.0f/255.0f, 1.0f})
           .SetAbsoluteSize({550, 220})
           .AlignTop()
           .AlignLeft(50);

    auto& speedWord = canvas->AddImageElement("Menu/TitleSpeed.png")
            .SetAbsoluteSize({290, 97})
            .SetAbsolutePosition({280, 0})
            .AlignTop(25);

    auto& funWord = canvas->AddImageElement("Menu/TitleFun.png")
            .SetAbsoluteSize({151, 77})
            .SetAbsolutePosition({407, 0})
            .AlignTop(122);

    auto& dashes = canvas->AddImageElement("Menu/Dashes.png")
            .SetAbsoluteSize({109, 70})
            .SetAbsolutePosition({285, 0})
            .AlignTop(128);

    hoverBox = &canvas->AddElement()
            .SetAbsoluteSize({460, 115})
            .SetColor({160.0f/255.0f, 20.0f / 255.0f, 220.0f/255.0f, 1.0f})
            .AlignTop(247)
            .AlignLeft(95);

    auto& singleplayer = canvas->AddImageElement("Menu/Singleplayer.png")
            .SetAbsoluteSize({315, 54})
            .SetAbsolutePosition({160, 0})
            .AlignTop(280);

    auto& multiplayer = canvas->AddImageElement("Menu/Multiplayer.png")
            .SetAbsoluteSize({290, 54})
            .SetAbsolutePosition({160, 0})
            .AlignTop(390);

    auto& exit = canvas->AddImageElement("Menu/Exit.png")
            .SetAbsoluteSize({97, 43})
            .SetAbsolutePosition({160, 0})
            .AlignTop(610);

    auto& options = canvas->AddImageElement("Menu/Options.png")
            .SetAbsoluteSize({201, 55})
            .SetAbsolutePosition({160, 0})
            .AlignTop(500);





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
