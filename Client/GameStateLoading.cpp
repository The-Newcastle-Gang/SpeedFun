#include "GameStateLoading.h"
using namespace NCL;
using namespace CSC8503;

Loading::Loading(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State() {
	renderer = rendererRef;
	world = gameWorldRef;
}

Loading::~Loading() {

}

void Loading::Update(float dt) {
	Debug::Print("LOADING", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO GAMEPLAY", Vector2(10, 20));
	Debug::Print("PRESS L TO MENU", Vector2(10, 30));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void Loading::OnEnter() {
	std::cout << "LOADING\n";
}

void Loading::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}