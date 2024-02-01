//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"
#include "enet.h"


void Server::ServerInit() {

    NetworkBase::Initialise();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 32);
}

void Server::UpdateServer(float dt) {
    serverBase->UpdateServer();
}