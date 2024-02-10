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

void Server::UpdateServer(float dt) {
    stateManager->Update(dt);
}
