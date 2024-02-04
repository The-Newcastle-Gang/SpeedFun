#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef) : State()
{
	renderer = rendererRef;
	world = gameWorldRef;
    baseClient = clientRef;
}

MenuState::~MenuState() {

}

bool MenuState::IsConnected() {
    return false;
}

void MenuState::Update(float dt) {

	Debug::Print("MENU", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO LOADING", Vector2(10, 20));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void MenuState::OnEnter() {
	std::cout << "MENU\n";
}

void MenuState::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}