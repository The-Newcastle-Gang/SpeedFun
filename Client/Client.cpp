//
// Created by c3042750 on 30/01/2024.
//

#include <NetworkObject.h>
#include "Client.h"

Client::Client() {
    NetworkBase::Initialise();

    baseClient = std::make_unique<GameClient>();
    stateManager = std::make_unique<StateMachine>();
    world = std::make_unique<GameWorld>();
    renderer = std::make_unique<GameTechRenderer>(*world);

    InitStateManager();
    RegisterPackets();
}

void Client::InitStateManager() {

    auto clientMenu = new MenuState(renderer.get(), world.get(), baseClient.get());
    auto clientGameplay = new GameplayState(renderer.get(), world.get(), baseClient.get());



    auto menuToGameplay = new StateTransition(clientMenu, clientGameplay, [=]()->bool {
        return clientMenu->CheckConnected();
    });

    auto gameplayToMenu = new StateTransition(clientGameplay, clientMenu, [=]()->bool {
        return clientGameplay->IsDisconnected();
    });

    stateManager->AddState(clientMenu);
    stateManager->AddState(clientGameplay);

    stateManager->AddTransition(menuToGameplay);
    stateManager->AddTransition(gameplayToMenu);

}

void Client::RegisterPackets() {
    baseClient->RegisterPacketHandler(Full_State, this);
    baseClient->RegisterPacketHandler(Function, this);
}

void Client::Update(float dt) {

    stateManager->Update(dt);
    baseClient->UpdateClient();
}


void Client::ReceivePacket(int type, GamePacket *payload, int source) {

    stateManager->ReceivePacket(type, payload, source);
}



