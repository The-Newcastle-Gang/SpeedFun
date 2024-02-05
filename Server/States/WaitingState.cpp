#include "WaitingState.h"


using namespace NCL;
using namespace CSC8503;

WaitingPlayers::WaitingPlayers(GameServer* pServerBase) : State() {
    serverBase = pServerBase;
}

WaitingPlayers::~WaitingPlayers() {

}

void WaitingPlayers::Update(float dt) {

}

void WaitingPlayers::AddPlayer(int peerId) {
    PlayerInfo info{};
    info.peerId = peerId;
    serverBase->AddPlayerInfo(peerId, info);
}

void WaitingPlayers::StartGame() {
    isReady = true;
}

bool WaitingPlayers::CheckPlayersReady() {
    return isReady;
}

void WaitingPlayers::ReceivePacket(int type, GamePacket *payload, int source) {
    switch(type) {
        case Function: {
            auto packet = reinterpret_cast<FunctionPacket*>(payload);
            if (packet->functionId == Replicated::StartGame) {
                StartGame();
            }
        } break;
    }
}

void WaitingPlayers::OnEnter() {
    std::cout << "Waiting for players" << std::endl;
    serverBase->OnPlayerJoined.connect<&WaitingPlayers::AddPlayer>(this);
    isReady = false;
}

void WaitingPlayers::OnExit() {
    serverBase->OnPlayerJoined.disconnect(this);
}
