#include "InactiveState.h"
using namespace NCL;
using namespace CSC8503;

InactiveState::InactiveState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State() {
	renderer = rendererRef;
	world = gameWorldRef;
}

InactiveState::~InactiveState() {

}

void InactiveState::Update(float dt) {
	Debug::Print("INACTIVE MENU", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO MENU", Vector2(10, 20));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void InactiveState::OnEnter() {
}

void InactiveState::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}