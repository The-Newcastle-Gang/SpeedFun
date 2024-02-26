#include "GameplayState.h"

using namespace NCL;
using namespace CSC8503;

GameplayState::GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Resources* pResources, Canvas* pCanvas) : State() {
    renderer = pRenderer;
    world = pGameworld;
    // Don't touch base client in here, need some way to protect this.
    baseClient = pClient;
    resources = pResources;
    replicated = std::make_unique<Replicated>();
    canvas = pCanvas;
}

GameplayState::~GameplayState() {
}


void GameplayState::InitCanvas(){

    //this is clearly not the best way to do the cross-heir but This will have to do for now
    //since i wanna just use this as debug.
    //I can bet money on the fact that this code is going to be at release
    //if u see this owen dont kill this

    auto crossHeirVert = canvas->AddElement()
            .SetColor({0.2,0.2,0.2,1.0})
            .SetAbsoluteSize({15,3})
            .AlignCenter()
            .AlignMiddle();

    auto crossHeirHoriz = canvas->AddElement()
            .SetColor({0.2,0.2,0.2,1.0})
            .SetAbsoluteSize({3,15})
            .AlignCenter()
            .AlignMiddle();
}

void GameplayState::ThreadUpdate(GameClient* client, ClientNetworkData* networkData) {

    auto threadClient = ClientThread(client, networkData);

    while (client) {
        threadClient.Update();
    }

}

void GameplayState::OnEnter() {
    firstPersonPosition = nullptr;
    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);
    CreateNetworkThread();
    InitialiseAssets();
    Window::GetWindow()->LockMouseToWindow(true);
    Window::GetWindow()->ShowOSPointer(false);
    InitCanvas();
}

void GameplayState::CreateNetworkThread() {
    GameClient* client = baseClient;
    baseClient = nullptr;
    networkData = std::make_unique<ClientNetworkData>();
    networkThread = new std::thread(ThreadUpdate, client, networkData.get());
    networkThread->detach();
}


void GameplayState::OnExit() {
    Window::GetWindow()->LockMouseToWindow(false);
    Window::GetWindow()->ShowOSPointer(true);
    world->ClearAndErase();
    renderer->Render();
    delete networkThread;
}

void GameplayState::Update(float dt) {
    SendInputData();
    ReadNetworkFunctions();

    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);

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
    InputListener::InputUpdate();
    InputPacket input;

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)){
        networkData->outgoingFunctions.Push(FunctionPacket(Replicated::PlayerJump, nullptr));
    }

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) {
        (*networkData).outgoingFunctions.Push(FunctionPacket(Replicated::RemoteServerCalls::PlayerGrapple, nullptr));
    }

    Camera* mainCamera = world->GetMainCamera();
    float cameraPitch = mainCamera->GetPitch();
    float cameraYaw = mainCamera->GetYaw();

    input.playerRotation = Quaternion::EulerAnglesToQuaternion(cameraPitch, cameraYaw, 0);

    Matrix4 camWorld = mainCamera->BuildViewMatrix().Inverse();
    input.rightAxis = Vector3(camWorld.GetColumn(0));


    input.playerDirection = InputListener::GetPlayerInput();

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
    InitLevel();
    CreatePlayers();
}

void GameplayState::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);
        player->SetRenderObject(new RenderObject(&player->GetTransform(), resources->GetMesh("Capsule.msh"), nullptr, nullptr));
    }
}

void GameplayState::InitLevel() {
    auto lr= new LevelReader();
    lr->HasReadLevel("debuglvl.json");
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