//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"
#include "enet.h"


void Server::ServerInit() {
    NetworkBase::Initialise();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 32);
    RegisterPackets();
}

void Server::RegisterPackets() {
    serverBase->RegisterPacketHandler(Received_State, this);
}

void Server::UpdateServer(float dt) {
    serverBase->UpdateServer();
}

void Server::ReceivePacket(int type, GamePacket *payload, int source) {
    switch (type) {
        case Received_State:
            auto packet = reinterpret_cast<InputPacket*>(payload);
            std::cout << packet->playerRotation << std::endl;
            std::cout << packet->playerDirection << std::endl;
    }
}
