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

	StateTransition* inactiveToLoading = new StateTransition(clientInactive, clientLoading, [&](void)->bool {
		return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::LoadingState);
		});
	StateTransition* loadingToMenu = new StateTransition(clientLoading, clientMenu, [&](void)->bool {
		return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::L), ClientStates::MenuState);
		});
	StateTransition* menuToLoading = new StateTransition(clientMenu, clientLoading, [&](void)->bool {
		return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::LoadingState);
		});
	StateTransition* loadingToInGameplay = new StateTransition(clientLoading, clientInGameplay, [&](void)->bool {
		return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::InGameplayState);
		});
	StateTransition* inGameplayToLoading = new StateTransition(clientInGameplay, clientLoading, [&](void)->bool {
		int tester = clientInGameplay->ExitType();
		return ForceTransition(tester != ExitStates::Invalid, ClientStates::LoadingState);
		});

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


bool ClientGameStateMachine::ForceTransition(bool hasConidtionMet, ClientStates stateSwitchTo) {
	if (hasConidtionMet) currentClientState = stateSwitchTo;
	return hasConidtionMet;
}