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

void Client::InitialiseAssets() {

}

std::string Client::GetAddress() {
    // TODO: Replace this with a way to input a certain address.
    return "127.0.0.1";
}

void Client::UpdateClient(float dt) {

    SendInputData();
    world->UpdateWorld(dt);
    renderer->Update(dt);
    renderer->Render();
    world->GetMainCamera()->UpdateCamera(dt);
    baseClient->UpdateClient();
}

void Client::SendInputData() {
    InputPacket input;

    auto mainCamera = world->GetMainCamera();
    auto cameraPitch = mainCamera->GetPitch();
    auto cameraYaw = mainCamera->GetYaw();

    input.playerRotation = Quaternion::EulerAnglesToQuaternion(cameraPitch, cameraYaw, 0);

    Vector2 playerDirection;

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
        playerDirection.y += 1;
    }
    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
        playerDirection.y += -1;
    }
    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
        playerDirection.x += -1;
    }
    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
        playerDirection.x += 1;
    }

    playerDirection.Normalise();
    input.playerDirection = playerDirection;

    baseClient->SendPacket(input);
}

void Client::ReceivePacket(int type, GamePacket *payload, int source) {

}




