//
// Created by c3042750 on 30/01/2024.
//

#include <NetworkObject.h>
#include "Client.h"

Client::Client() {
    NetworkBase::Initialise();
    replicated = std::make_unique<Replicated>();
    baseClient = std::make_unique<GameClient>();
    world = std::make_unique<GameWorld>();
    renderer = std::make_unique<GameTechRenderer>(*world);

    thisPlayer = nullptr;
}

void Client::InitClient() {
    InitialiseAssets();
    InitNetworking();
    InitGame();
}

void Client::InitNetworking() {
    auto address = GetAddress();
    baseClient->Connect(address, NetworkBase::GetDefaultPort());

    RegisterPackets();

    std::cout << "Client starting up!" << std::endl;
}

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
    CreatePlayers();
}

std::string Client::GetAddress() {
    // TODO: Replace this with a way to input a certain address.
    return "127.0.0.1";
}

void Client::Update(float dt) {

    SendInputData();
    world->UpdateWorld(dt);
    renderer->Update(dt);
    renderer->Render();
    world->GetMainCamera()->UpdateCamera(dt);
    if (thisPlayer) {
        world->GetMainCamera()->SetPosition(thisPlayer->GetTransform().GetPosition());
    }
    baseClient->UpdateClient();
}

// Not saying we should keep this (though we can) just wanted to get a demonstration.
void Client::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        // Raise discussion on how to manage gameworld entities and who should own their lifecycle.
        auto* player = new GameObject();
        replicated->CreatePlayer(player);
        player->SetRenderObject(new RenderObject(&player->GetTransform(), GetMesh("Goat.msh"), nullptr, nullptr));
        world->AddGameObject(player);
    }
}

void Client::AssignPlayer(unsigned char* data) {
    auto playerObject = replicated->networkObjects[*((int*)data)]->GetParent();
    thisPlayer = playerObject;
    playerObject->SetRenderObject(nullptr);
}

void Client::SendInputData() {
    InputPacket input;

    Camera* mainCamera = world->GetMainCamera();
    float cameraPitch = mainCamera->GetPitch();
    float cameraYaw = mainCamera->GetYaw();

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
    switch (type) {
        case Full_State: {
            auto* packet = reinterpret_cast<FullPacket*>(payload);
            replicated->networkObjects[packet->objectID]->ReadPacket(*payload);

        } break;

        case Function: {
            auto* packet = reinterpret_cast<FunctionPacket*>(payload);
            if (packet->functionId == Replicated::AssignPlayer) {
                AssignPlayer(packet->data.data);
            }

        } break;
    }
}

MeshGeometry *Client::GetMesh(const std::string& name) {
    if (meshes.find(name) == meshes.end()) {
        MeshGeometry* mesh;
        mesh = renderer->LoadMesh(name);
        meshes.insert(std::make_pair(name, mesh));
        return mesh;
    }

    return meshes[name];
}

ShaderBase *Client::GetShader(const std::string& shader) {
    if (shaders.find(shader) == shaders.end()) {
        auto p = renderer->LoadShader(shader + std::string(".vert"), shader + std::string(".frag"));
        shaders.insert(std::make_pair(shader, p));
        return p;
    }

    return shaders[shader];
}




