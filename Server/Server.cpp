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

//void Server::AssignPlayer(int peerId) {
//    auto functionData = std::make_unique<FunctionData>();
//    unsigned char* address = functionData->data;
//    *((int*)address) = GetPlayerFromPeerId(peerId)->GetNetworkObject()->GetNetworkId();
//    SendFunction(peerId, Replicated::AssignPlayer, functionData.get());
//}

//void Server::SendFunction(int peerId, int functionId, FunctionData *data) {
//    auto packet = std::make_unique<FunctionPacket>(functionId, data);
//    serverBase->SendPacket(*packet, peerId);
//    std::cout << "Sent function" << std::endl;
//}

//// See comments on client side.
//void Server::CreatePlayers() {
//    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
//        auto player = new GameObject();
//        replicated->CreatePlayer(player);
//
//        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
//        player->GetPhysicsObject()->InitSphereInertia();
//
//        players[i] = player;
//        //world->AddGameObject(player);
//    }
//}


void Server::RegisterPackets() {
    serverBase->RegisterPacketHandler(Received_State, this);
    serverBase->RegisterPacketHandler(Function, this);
}

void Server::UpdateServer(float dt) {
    stateManager->Update(dt);
    serverBase->UpdateServer();
}


void Server::ReceivePacket(int type, GamePacket *payload, int source) {
    stateManager->ReceivePacket(type, payload, source);
}
