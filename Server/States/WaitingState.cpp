#include "WaitingState.h"


using namespace NCL;
using namespace CSC8503;

WaitingPlayers::WaitingPlayers(GameServer* pServerBase) : State() {
    serverBase = pServerBase;
}

WaitingPlayers::~WaitingPlayers() {

}

void WaitingPlayers::RegisterPackets() {
    serverBase->RegisterPacketHandler(Function, this);
}

void WaitingPlayers::Update(float dt) {
    serverBase->UpdateServer();
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
            else if (packet->functionId == Replicated::SetLevel) {
                DataHandler handler(&packet->data);
                selectedLevel = handler.Unpack<int>();
                std::cout << selectedLevel<<" SELECTED!!\n";
            }
        } break;
    }
}

void WaitingPlayers::OnEnter() {
    std::cout << "Waiting for players" << std::endl;
    serverBase->OnPlayerJoined.connect<&WaitingPlayers::AddPlayer>(this);
    isReady = false;
    RegisterPackets();
}

void WaitingPlayers::OnExit() {
    serverBase->OnPlayerJoined.disconnect(this);
    serverBase->ClearPacketHandlers();
}
