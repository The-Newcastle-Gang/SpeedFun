#include "WaitingState.h"
#include "RendererBase.h"

using namespace NCL;
using namespace CSC8503;

WaitingPlayers::WaitingPlayers(GameServer* pServerBase) : State() {
    serverBase = pServerBase;
}

WaitingPlayers::~WaitingPlayers() {

}

void WaitingPlayers::Update(float dt) {

}