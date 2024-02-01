//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"


void Server::ServerInit() {
    NetworkBase::Initialise();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 4);
    std::cout << "Server starting up" << std::endl;

}

void Server::UpdateServer(float dt) {

    serverBase->UpdateServer();
}