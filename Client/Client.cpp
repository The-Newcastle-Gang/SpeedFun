//
// Created by c3042750 on 30/01/2024.
//

#include <NetworkObject.h>
#include "Client.h"

Client::Client() {
    thisPlayer = nullptr;
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

void Client::InitClient() {
    InitialiseAssets();
    InitNetworking();
    InitGame();
}

void Client::InitNetworking() {}

void Client::InitGame() {
    InitCamera();
}

void Client::RegisterPackets() {
    baseClient->RegisterPacketHandler(Full_State, this);
    baseClient->RegisterPacketHandler(Function, this);
}

void Client::InitCamera() {
    world->GetMainCamera()->SetNearPlane(0.1f);
    world->GetMainCamera()->SetFarPlane(500.0f);
    world->GetMainCamera()->SetPitch(-15.0f);
    world->GetMainCamera()->SetYaw(315.0f);
    world->GetMainCamera()->SetPosition(Vector3( -60, 40, 60));
}

void Client::InitialiseAssets() {
    TemporaryLevelLoad();
}

// Remove this when level loading is introduced.
void Client::TemporaryLevelLoad() {
//    CreatePlayers();
}

std::string Client::GetAddress() {
    // TODO: Replace this with a way to input a certain address.
    return "127.0.0.1";
}

void Client::Update(float dt) {

    stateManager->Update(dt);

//    SendInputData();
//    world->UpdateWorld(dt);
//    renderer->Update(dt);
//    renderer->Render();
//    world->GetMainCamera()->UpdateCamera(dt);
//    if (thisPlayer) {
//        world->GetMainCamera()->SetPosition(thisPlayer->GetTransform().GetPosition());
//    }
    baseClient->UpdateClient();
}

// Not saying we should keep this (though we can) just wanted to get a demonstration.

//void Client::AssignPlayer(unsigned char* data) {
//    auto playerObject = replicated->networkObjects[*((int*)data)]->GetParent();
//    thisPlayer = playerObject;
//    playerObject->SetRenderObject(nullptr);
//}


void Client::ReceivePacket(int type, GamePacket *payload, int source) {

    stateManager->ReceivePacket(type, payload, source);

//    switch (type) {
//        case Full_State: {
//            auto packet = reinterpret_cast<FullPacket*>(payload);
//            replicated->networkObjects[packet->objectID]->ReadPacket(*payload);
//
//        } break;
//
//        case Function: {
//            auto packet = reinterpret_cast<FunctionPacket*>(payload);
//            if (packet->functionId == Replicated::AssignPlayer) {
//                AssignPlayer(packet->data.data);
//            }
//
//        } break;
//    }
}



