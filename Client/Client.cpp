//
// Created by c3042750 on 30/01/2024.
//

#include "Client.h"


Client::Client() {
    NetworkBase::Initialise();

    stateManager = std::make_unique<StateMachine>();
    world = std::make_unique<GameWorld>();
    renderer = std::make_unique<GameTechRenderer>(*world);
    networkData = std::make_unique<ClientNetworkData>();
    baseClient = std::make_unique<GameClient>();

    InitStateManager();
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

void Client::Update(float dt) {

    //baseClient->UpdateClient();

    stateManager->Update(dt);

    if (dt > 0.04) {
        std::cout << "Overly large dt" << std::endl;
    }
}



