//
// Created by c3042750 on 30/01/2024.
//

#include "Client.h"

void Client::InitClient() {
    InitNetworking();
    InitGame();
}

void Client::InitNetworking() {
    NetworkBase::Initialise();

    auto address = GetAddress();

    baseClient = std::make_unique<GameClient>();
    baseClient->Connect(address, NetworkBase::GetDefaultPort());

    std::cout << "Client starting up!" << std::endl;
}

void Client::InitGame() {
    world = std::make_unique<GameWorld>();
    renderer = std::make_unique<GameTechRenderer>(*world);
    InitCamera();
}

void Client::InitCamera() {
    world->GetMainCamera()->SetNearPlane(0.1f);
    world->GetMainCamera()->SetFarPlane(500.0f);
    world->GetMainCamera()->SetPitch(-15.0f);
    world->GetMainCamera()->SetYaw(315.0f);
    world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

std::string Client::GetAddress() {
    // TODO: Replace this with a way to input a certain address.
    return "127.0.0.1";
}

void Client::UpdateClient(float dt) {
    baseClient->UpdateClient();
}
