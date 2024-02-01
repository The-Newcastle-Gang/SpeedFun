#include "GameStateMenu.h"
using namespace NCL;
using namespace CSC8503;

Menu::Menu(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State()
{
	renderer = rendererRef;
	world = gameWorldRef;
}

Menu::~Menu() {

}

void Menu::Update(float dt) {
	std::cout << "MENU\n";
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void Menu::OnEnter() {
}

void Menu::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}