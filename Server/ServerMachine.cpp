#include "ServerMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameStateBusy.h"
#include "GameStateWaitingPlayers.h"
#include "GameStateRunning.h"
using namespace NCL;
using namespace CSC8503;
ServerMachine::ServerMachine() : StateMachine() {
    currentServerState = ServerStates::BusyState;
    InitialiseServerStateMachine();
}

void ServerMachine::InitialiseServerStateMachine() {
    auto serverBusy = new Busy();
    auto serverWaitingPlayers = new WaitingPlayers();
    auto serverRunning = new Running();

    auto busyToWaiting = new StateTransition(serverBusy, serverWaitingPlayers, [&]() {
        return currentServerState == ServerStates::WaitingPlayersState;
    });
    auto waitingToBusy = new StateTransition(serverWaitingPlayers, serverBusy, [&]() {
        return currentServerState == ServerStates::BusyState;
    });
    auto waitingToRunning = new StateTransition(serverWaitingPlayers, serverRunning, [&]() {
        return currentServerState == ServerStates::RunningState;
    });
    auto runningToWaiting = new StateTransition(serverRunning, serverWaitingPlayers, [&]() {
        return currentServerState == ServerStates::WaitingPlayersState;
    });

    this->AddState(serverBusy);
    this->AddState(serverWaitingPlayers);
    this->AddState(serverRunning);

    this->AddTransition(busyToWaiting);
    this->AddTransition(waitingToBusy);
    this->AddTransition(waitingToRunning);
    this->AddTransition(runningToWaiting);
}