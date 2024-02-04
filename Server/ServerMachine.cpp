#include "ServerMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "BusyState.h"
#include "WaitingState.h"
#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;
ServerMachine::ServerMachine() : StateMachine() {
    currentServerState = ServerStates::Busy;
    InitMachine();
}

void ServerMachine::InitMachine() {

}