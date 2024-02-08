#include "GameplayState.h"

using namespace NCL;
using namespace CSC8503;

GameplayState::GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient) : State() {
    renderer = pRenderer;
    world = pGameworld;
    baseClient = pClient;
    resources = std::make_unique<Resources>(renderer);
}

GameplayState::~GameplayState() {}

void GameplayState::OnEnter() {

    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);

    firstPersonPosition = nullptr;
    InitialiseAssets();
}
void GameplayState::OnExit() {
    world->ClearAndErase();
    renderer->Render();
}

void GameplayState::Update(float dt) {

    if (firstPersonPosition) {
        world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
    }

    world->GetMainCamera()->UpdateCamera(dt);
    SendInputData();
    world->UpdateWorld(dt);
    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void GameplayState::SendInputData() {
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


void GameplayState::InitialiseAssets() {
    InitCamera();
    InitWorld();
    FinishLoading();
}

void GameplayState::FinishLoading() {
    baseClient->RemoteFunction(Replicated::GameLoaded, nullptr);
}

void GameplayState::InitCamera() {
    world->GetMainCamera()->SetNearPlane(0.1f);
    world->GetMainCamera()->SetFarPlane(500.0f);
    world->GetMainCamera()->SetPitch(-15.0f);
    world->GetMainCamera()->SetYaw(315.0f);
    world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void GameplayState::InitWorld() {
    CreatePlayers();
}

void GameplayState::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);
        player->SetRenderObject(new RenderObject(&player->GetTransform(), resources->GetMesh("Goat.msh"), nullptr, nullptr));
    }
}


bool GameplayState::IsDisconnected() {
    return false;
}


void GameplayState::AssignPlayer(int netObject) {
    auto player = world->GetObjectByNetworkId(netObject);
    player->SetRenderObject(nullptr);
    firstPersonPosition = &player->GetTransform();
}

void GameplayState::ReceivePacket(int type, GamePacket *payload, int source) {
    if (type == Function) {
        auto functionPacket = reinterpret_cast<FunctionPacket*>(payload);
        if (functionPacket->functionId == Replicated::AssignPlayer) {
            DataHandler handler(&functionPacket->data);
            auto networkId = handler.Unpack<int>();
            AssignPlayer(networkId);
        }
    }
}
