#include "LoadingState.h"
using namespace NCL;
using namespace CSC8503;

LoadingState::LoadingState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef) : State() {
	renderer = rendererRef;
	world = gameWorldRef;
    baseClient = clientRef;
}

LoadingState::~LoadingState() {

}

void LoadingState::Update(float dt) {
	Debug::Print("LOADING", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO GAMEPLAY", Vector2(10, 20));
	Debug::Print("PRESS L TO MENU", Vector2(10, 30));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void LoadingState::OnEnter() {
	std::cout << "LOADING" << std::endl;
}

void LoadingState::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}