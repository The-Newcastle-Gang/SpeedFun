#include "GameplayState.h"

using namespace NCL;
using namespace CSC8503;

GameplayState::GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient) : State() {
    renderer = pRenderer;
    world = pGameworld;
    // Don't touch base client in here, need some way to protect this.
    baseClient = pClient;
    resources = std::make_unique<Resources>(renderer);
    replicated = std::make_unique<Replicated>();
}

GameplayState::~GameplayState() {
    delete networkThread;
}

void GameplayState::ThreadUpdate(GameClient* client, ClientNetworkData* networkData) {

    auto threadClient = ClientThread(client, networkData);

    while (client) {
        threadClient.Update();
    }

}

void GameplayState::OnEnter() {
    firstPersonPosition = nullptr;
    CreateNetworkThread();
    InitialiseAssets();
}

void GameplayState::CreateNetworkThread() {
    GameClient* client = baseClient;
    baseClient = nullptr;
    networkData = std::make_unique<ClientNetworkData>();
    networkThread = new std::thread(ThreadUpdate, client, networkData.get());
    networkThread->detach();
}


void GameplayState::OnExit() {
    world->ClearAndErase();
    renderer->Render();
}

void GameplayState::Update(float dt) {
    SendInputData();
    ReadNetworkFunctions();

    if (firstPersonPosition) {
        world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
    }

    world->GetMainCamera()->UpdateCamera(dt);
    world->UpdateWorld(dt);

    ReadNetworkPackets();

    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void GameplayState::ReadNetworkFunctions() {
    while (!networkData->incomingFunctions.IsEmpty()) {
        FunctionPacket packet = networkData->incomingFunctions.Pop();
        if (packet.functionId == Replicated::AssignPlayer) {
            DataHandler handler(&packet.data);
            auto networkId = handler.Unpack<int>();
            AssignPlayer(networkId);
        }
    }
}

// Perhaps replace this with a data structure that won't overlap objects on the same packet.
void GameplayState::ReadNetworkPackets() {
    while (!networkData->incomingState.IsEmpty()) {
        FullPacket packet = networkData->incomingState.Pop();
        auto id = packet.objectID;
        world->GetNetworkObject(id)->ReadPacket(packet);
    }
}

void GameplayState::SendInputData() {
    InputPacket input;

    Camera* mainCamera = world->GetMainCamera();
    float cameraPitch = mainCamera->GetPitch();
    float cameraYaw = mainCamera->GetYaw();

    input.playerRotation = Quaternion::EulerAnglesToQuaternion(cameraPitch, cameraYaw, 0);

    Matrix4 camWorld = mainCamera->BuildViewMatrix().Inverse();
    input.rightAxis = Vector3(camWorld.GetColumn(0));
    input.fwdAxis = Vector3::Cross(Vector3(0,1,0), input.rightAxis);



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


    networkData->outgoingInput.Push(input);
}


void GameplayState::InitialiseAssets() {
    InitCamera();
    InitWorld();
    FinishLoading();
}

void GameplayState::FinishLoading() {
    networkData->outgoingFunctions.Push(FunctionPacket(Replicated::GameLoaded, nullptr));
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
    InitLevel();
}

void GameplayState::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);
        player->SetRenderObject(new RenderObject(&player->GetTransform(), resources->GetMesh("Goat.msh"), nullptr, nullptr));
    }
}

void GameplayState::InitLevel(){

    auto lr= new LevelReader();
    lr->HasReadLevel("finaltest.json");
    auto plist  = lr->GetPrimitiveList();
    for(auto x : plist){
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
    }
}


bool GameplayState::IsDisconnected() {
    return false;
}


void GameplayState::AssignPlayer(int netObject) {
    auto player = world->GetObjectByNetworkId(netObject);
    player->SetRenderObject(nullptr);
    firstPersonPosition = &player->GetTransform();
    std::cout << "Assigning player to network object: " << player->GetNetworkObject()->GetNetworkId() << std::endl;

}