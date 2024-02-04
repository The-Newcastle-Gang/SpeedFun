#include "ClientMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "InactiveState.h"
#include "LoadingState.h"
#include "MenuState.h"
#include "GameplayState.h"

using namespace NCL;
using namespace CSC8503;

ClientMachine::ClientMachine() : StateMachine() {
    gameWorld = new GameWorld();
    renderer = new GameTechRenderer(*gameWorld);
    currentClientState = ClientStates::InactiveState;
    InitialiseClientStateMachine();
}

void ClientMachine::InitialiseClientStateMachine() {
    auto clientInactive = new InactiveState(renderer, gameWorld);
    auto clientLoading = new LoadingState(renderer, gameWorld);
    auto clientMenu = new MenuState(renderer, gameWorld);
    auto clientInGameplay = new GameplayState(renderer, gameWorld);

    auto inactiveToLoading = new StateTransition(clientInactive, clientLoading, [&]() {
        return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::LoadingState);
    });
    auto loadingToMenu = new StateTransition(clientLoading, clientMenu, [&]() {
        return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::L), ClientStates::MenuState);
    });
    auto menuToLoading = new StateTransition(clientMenu, clientLoading, [&](){
        return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::LoadingState);
    });
    auto loadingToInGameplay = new StateTransition(clientLoading, clientInGameplay, [&]() {
        return ForceTransition(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE), ClientStates::InGameplayState);
    });
    auto inGameplayToLoading = new StateTransition(clientInGameplay, clientLoading, [&]() {
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


bool ClientMachine::ForceTransition(bool hasConditionMet, ClientStates stateSwitchTo) {
    if (hasConditionMet) currentClientState = stateSwitchTo;
    return hasConditionMet;
}