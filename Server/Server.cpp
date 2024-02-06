//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"
#include "enet.h"

Server::Server() {
    NetworkBase::Initialise();
    stateManager = std::make_unique<StateMachine>();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 32);
    InitStateMachine();
    RegisterPackets();
}

Server::~Server() {

}

void Server::InitStateMachine() {
    auto waitingPlayers = new WaitingPlayers(serverBase.get());
    auto running = new RunningState(serverBase.get());

    stateManager->AddState(waitingPlayers);
    stateManager->AddState(running);

    stateManager->AddTransition(new StateTransition(waitingPlayers, running, [=]()->bool {
        return waitingPlayers->CheckPlayersReady();
    }));
}

void Server::RegisterPackets() {
    serverBase->RegisterPacketHandler(Received_State, this);
    serverBase->RegisterPacketHandler(Function, this);
}

void Server::UpdateServer(float dt) {
    std::cout << "Server update loop. \n";
    stateManager->Update(dt);
    serverBase->UpdateServer();
}


void Server::ReceivePacket(int type, GamePacket *payload, int source) {
    stateManager->ReceivePacket(type, payload, source);
}
