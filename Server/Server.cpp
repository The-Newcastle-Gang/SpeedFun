//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"
#include "enet.h"


void Server::ServerInit() {
    NetworkBase::Initialise();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 32);
    sceneSnapshotId = 0;
    packetTimer = SERVERHERTZ;
    RegisterPackets();
    InitGame();
}

void Server::InitGame() {
    world = std::make_unique<GameWorld>();
    physics = std::make_unique<PhysicsSystem>(*world);

}



// These will be like a pipeline function.
void Server::CreateServerObject(GameObject* g) {
    Replicated::CreateObject(g, *world);

}

void Server::RegisterPackets() {
    serverBase->RegisterPacketHandler(Received_State, this);
}

void Server::UpdateServer(float dt) {
    world->UpdateWorld(dt);
    physics->Update(dt);
    Tick(dt);
    serverBase->UpdateServer();
}

void Server::Tick(float dt) {
    packetTimer -= dt;
    if (packetTimer < 0) {
        SendWorldToClient();
        packetTimer += SERVERHERTZ;
    }
}

void Server::SendWorldToClient() {
    world->OperateOnContents([this](GameObject* obj) {
        if (!obj->IsActive()) {
            return;
        }

        auto netObj = obj->GetNetworkObject();
        if (!netObj) {
            return;
        }

        GamePacket* newPacket = nullptr;
        if (netObj->WritePacket(&newPacket, false, sceneSnapshotId)) {
            serverBase->SendGlobalPacket(*newPacket);
        }
        delete newPacket;
    });
}

void Server::ReceivePacket(int type, GamePacket *payload, int source) {
    switch (type) {
        case Received_State: {
            auto packet = reinterpret_cast<InputPacket*>(payload);
            std::cout << packet->playerRotation << std::endl;
            std::cout << packet->playerDirection << std::endl;
            break;
        }
    }
}
