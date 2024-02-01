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
	
	Debug::Print("MENU", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO LOADING", Vector2(10, 20));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void Menu::OnEnter() {
	std::cout << "MENU\n";
}

void Menu::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}
