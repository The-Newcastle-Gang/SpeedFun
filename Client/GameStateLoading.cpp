#include "GameStateLoading.h"
using namespace NCL;
using namespace CSC8503;

Loading::Loading(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State()
{
	renderer = rendererRef;
	world = gameWorldRef;
}

Loading::~Loading()
{

}

void Loading::Update(float dt)
{
	std::cout << "LOADING\n";
	renderer->Render();
	Debug::UpdateRenderables(dt);
}
void Loading::OnEnter()
{
}
void Loading::OnExit()
{
	world->ClearAndErase();
	renderer->Render();
}