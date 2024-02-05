#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient) : State()
{
    renderer = pRenderer;
	world = pGameworld;
    baseClient = pClient;
}

void MenuState::OnEnter() {
    isGameStarted = false;
    statusText = "Press L to connect to localhost";
    connectState = 0;

}

MenuState::~MenuState() {

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
    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L) && connectState == 0) {
        ConnectToGame("127.0.0.1");
    }

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R) && connectState == 2) {
        StartGame();
    }

	Debug::Print(statusText, Vector2(10, 20));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void MenuState::ReceivePacket(int type, GamePacket *payload, int source) {

}



void MenuState::OnExit() {
    baseClient->OnServerConnected.disconnect(this);
}

