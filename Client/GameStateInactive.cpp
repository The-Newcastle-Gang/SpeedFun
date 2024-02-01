#include "GameStateInactive.h"
using namespace NCL;
using namespace CSC8503;

Inactive::Inactive(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State()
{
	renderer = rendererRef;
	world = gameWorldRef;
}

Inactive::~Inactive()
{

}

void Inactive::Update(float dt)
{
	std::cout << "INACTIVE\n";
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void Inactive::OnEnter()
{
}
void Inactive::OnExit()
{
	world->ClearAndErase();
	renderer->Render();
}