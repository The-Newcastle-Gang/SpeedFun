#include "ClientGameStateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameStateInactive.h"
#include "GameStateLoading.h"
#include "GameStateMenu.h"
#include "GameStateInGameplay.h"
using namespace NCL;
using namespace CSC8503;
ClientGameStateMachine::ClientGameStateMachine() : StateMachine()
{
	currentClientState = ClientStates::Inactive;
	InitialiseClientStateMachine();
}

ClientGameStateMachine::~ClientGameStateMachine()
{
}

void ClientGameStateMachine::InitialiseClientStateMachine()
{
	//*
	GameStateInactive* clientInactive = new GameStateInactive();
	GameStateLoading* clientLoading = new GameStateLoading();
	GameStateMenu* clientMenu = new GameStateMenu();
	GameStateInGameplay* clientInGameplay = new GameStateInGameplay();

	StateTransition* inactiveToLoading = new StateTransition(clientInactive, clientLoading, [&](void)->bool { return currentClientState == ClientStates::Loading; });
	StateTransition* loadingToMenu = new StateTransition(clientLoading, clientMenu, [&](void)->bool { return currentClientState == ClientStates::Menu; });
	StateTransition* menuToLoading = new StateTransition(clientMenu, clientLoading, [&](void)->bool { return currentClientState == ClientStates::Loading; });
	StateTransition* loadingToInGameplay = new StateTransition(clientLoading, clientInGameplay, [&](void)->bool { return currentClientState == ClientStates::InGameplay; });
	StateTransition* inGameplayToLoading = new StateTransition(clientInGameplay, clientLoading, [&](void)->bool { return currentClientState == ClientStates::Loading; });

	this->AddState(clientInactive);
	this->AddState(clientLoading);
	this->AddState(clientMenu);
	this->AddState(clientInGameplay);

	this->AddTransition(inactiveToLoading);
	this->AddTransition(loadingToMenu);
	this->AddTransition(menuToLoading);
	this->AddTransition(loadingToInGameplay);
	this->AddTransition(inGameplayToLoading);
	//*/
}
