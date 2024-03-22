//
// Created by c3042750 on 30/01/2024.
//

#include "Client.h"
#include "../CSC8503CoreClasses/Debug.h"

Client::Client(std::atomic<bool> &shouldStart) : serverStart(shouldStart), baseClient(std::make_unique<GameClient>()), stateManager(std::make_unique<StateMachine>()) {
    NetworkBase::Initialise();
    soundManager = std::make_unique<SoundManager>();

    world = std::make_unique<GameWorld>();
    canvas = std::make_unique<Canvas>();
    renderer = std::make_unique<GameTechRenderer>(*world, *canvas);
    resources = std::make_unique<Resources>(renderer.get());

    
    InitStateManager();
}

Client::~Client() {
    //std::cout << "Client shutting down" << std::endl;
}

void Client::InitStateManager() {

    auto clientMenu = new MenuState(renderer.get(), world.get(), baseClient.get(), canvas.get(), soundManager.get(), serverStart);
    auto clientGameplay = new GameplayState(renderer.get(), world.get(), baseClient.get(), resources.get(), canvas.get(), soundManager.get());

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

    stateManager->Update(dt);

    canvas->Update(dt);

    if (dt > 0.04) {
        //std::cout << "Overly large dt" << std::endl;
    }
}



