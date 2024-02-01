#include "ClientGameStateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "./../Client/GameStateInactive.h"
#include "./../Client/GameStateLoading.h"
#include "./../Client/GameStateMenu.h"
#include "./../Client/GameStateInGameplay.h"
using namespace NCL;
using namespace CSC8503;
ClientGameStateMachine::ClientGameStateMachine() : StateMachine()
{
	gameWorld = new GameWorld();
	renderer = new GameTechRenderer(*gameWorld);
	currentClientState = ClientStates::InactiveState;
	InitialiseClientStateMachine();
}

ClientGameStateMachine::~ClientGameStateMachine()
{
}

void ClientGameStateMachine::InitialiseClientStateMachine()
{
	//*
	Inactive* clientInactive = new Inactive(renderer, gameWorld);
	Loading* clientLoading = new Loading(renderer, gameWorld);
	Menu* clientMenu = new Menu(renderer, gameWorld);
	InGameplay* clientInGameplay = new InGameplay(renderer, gameWorld);

	StateTransition* inactiveToLoading = new StateTransition(clientInactive, clientLoading, [&](void)->bool { return currentClientState == ClientStates::LoadingState; });
	StateTransition* loadingToMenu = new StateTransition(clientLoading, clientMenu, [&](void)->bool { return currentClientState == ClientStates::MenuState; });
	StateTransition* menuToLoading = new StateTransition(clientMenu, clientLoading, [&](void)->bool { return currentClientState == ClientStates::LoadingState; });
	StateTransition* loadingToInGameplay = new StateTransition(clientLoading, clientInGameplay, [&](void)->bool { return currentClientState == ClientStates::InGameplayState; });
	StateTransition* inGameplayToLoading = new StateTransition(clientInGameplay, clientLoading, [&](void)->bool { return currentClientState == ClientStates::LoadingState; });

	this->AddState(clientInactive);
	this->AddState(clientLoading);
	this->AddState(clientMenu);
	this->AddState(clientInGameplay);

	this->AddTransition(inactiveToLoading);
	this->AddTransition(loadingToMenu);
	this->AddTransition(menuToLoading);
	this->AddTransition(loadingToInGameplay);
	this->AddTransition(inGameplayToLoading);
}
