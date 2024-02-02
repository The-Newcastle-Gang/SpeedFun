#include "ServerGameStateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameStateBusy.h"
#include "GameStateWaitingPlayers.h"
#include "GameStateRunning.h"
using namespace NCL;
using namespace CSC8503;
ServerGameStateMachine::ServerGameStateMachine() : StateMachine()
{
	currentServerState = ServerStates::BusyState;
	InitialiseServerStateMachine();
}

ServerGameStateMachine::~ServerGameStateMachine()
{
}

void ServerGameStateMachine::InitialiseServerStateMachine()
{
	Busy* serverBusy = new Busy();
	WaitingPlayers* serverWaitingPlayers = new WaitingPlayers();
	Running* serverRunning = new Running();

	StateTransition* busyToWaiting = new StateTransition(serverBusy, serverWaitingPlayers, [&](void)->bool { return currentServerState == ServerStates::WaitingPlayersState; });
	StateTransition* waitingToBusy = new StateTransition(serverWaitingPlayers, serverBusy, [&](void)->bool { return currentServerState == ServerStates::BusyState; });
	StateTransition* waitingToRunning = new StateTransition(serverWaitingPlayers, serverRunning, [&](void)->bool { return currentServerState == ServerStates::RunningState; });
	StateTransition* runningToWaiting = new StateTransition(serverRunning, serverWaitingPlayers, [&](void)->bool { return currentServerState == ServerStates::WaitingPlayersState; });

	this->AddState(serverBusy);
	this->AddState(serverWaitingPlayers);
	this->AddState(serverRunning);

	this->AddTransition(busyToWaiting);
	this->AddTransition(waitingToBusy);
	this->AddTransition(waitingToRunning);
	this->AddTransition(runningToWaiting);
}