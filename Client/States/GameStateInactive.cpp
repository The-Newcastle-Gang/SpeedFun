#include "GameStateInactive.h"
using namespace NCL;
using namespace CSC8503;

Inactive::Inactive(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State() {
	renderer = rendererRef;
	world = gameWorldRef;
}

Inactive::~Inactive() {

}

void Inactive::Update(float dt) {
	Debug::Print("INACTIVE MENU", Vector2(10, 10));
	Debug::Print("PRESS SPACE TO MENU", Vector2(10, 20));
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void Inactive::OnEnter() {
}

void Inactive::OnExit() {
	world->ClearAndErase();
	renderer->Render();
}