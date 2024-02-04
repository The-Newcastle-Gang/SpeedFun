//
// Created by c3042750 on 30/01/2024.
//

#include "Server.h"
#include "enet.h"

Server::Server() {
    NetworkBase::Initialise();
    stateManager = std::make_unique<StateMachine>();
    serverBase = std::make_unique<GameServer>(NetworkBase::GetDefaultPort(), 32);
    replicated = std::make_unique<Replicated>();
    InitStateMachine();
    RegisterPackets();
}

Server::~Server() {

}

void Server::InitStateMachine() {
    auto serverWaitingPlayers = new WaitingPlayers(serverBase.get());
    auto serverRunning = new Running(serverBase.get());


    stateManager->AddState(serverWaitingPlayers);
    stateManager->AddState(serverRunning);

}

void Server::ServerInit() {
    serverBase->OnPlayerJoined.connect<&Server::AssignPlayer>(this);
    sceneSnapshotId = 0;
    packetTimer = SERVERHERTZ;
    InitGame();
}

void Server::InitGame() {
    world = std::make_unique<GameWorld>();
    physics = std::make_unique<PhysicsSystem>(*world);
    CreatePlayers();
}

void Server::CreateServerObject(GameObject *g) {

}

void Server::AssignPlayer(int peerId) {
    auto functionData = std::make_unique<FunctionData>();
    unsigned char* address = functionData->data;
    *((int*)address) = GetPlayerFromPeerId(peerId)->GetNetworkObject()->GetNetworkId();
    SendFunction(peerId, Replicated::AssignPlayer, functionData.get());
}

void Server::SendFunction(int peerId, int functionId, FunctionData *data) {
    auto packet = std::make_unique<FunctionPacket>(functionId, data);
    serverBase->SendPacket(*packet, peerId);
    std::cout << "Sent function" << std::endl;
}

// See comments on client side.
void Server::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
        player->GetPhysicsObject()->InitSphereInertia();

        players[i] = player;
        world->AddGameObject(player);
    }
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
    // Test swapping this with iterators instead of lambda callback for performance.
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
            GameObject* player = GetPlayerFromPeerId(source);
            player->GetTransform().SetOrientation(packet->playerRotation);
            break;
        }
    }
}

// This is dangerous, I'm not sure if enet will stack peer ids like 0,1,2,3? Array could break.
GameObject *Server::GetPlayerFromPeerId(int peerId) {
    return players[peerId];
}


