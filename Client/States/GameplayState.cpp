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

    timeBar = new Element(1);
}

GameplayState::~GameplayState() {
}


void GameplayState::InitCanvas(){

    //this is clearly not the best way to do the cross-heir but This will have to do for now
    //since i wanna just use this as debug.
    //I can bet money on the fact that this code is going to be at release
    //if u see this owen dont kill this

    InitCrossHeir();
    InitTimerBar();
    InitLevelMap();
}

void GameplayState::InitCrossHeir(){
    //crossheir
    auto crossHeirVert = canvas->AddElement()
            .SetColor({0.0,0.0,0.0,1.0})
            .SetAbsoluteSize({15,3})
            .AlignCenter()
            .AlignMiddle();

    auto crossHeirHoriz = canvas->AddElement()
            .SetColor({0.0,0.0,0.0,1.0})
            .SetAbsoluteSize({3,15})
            .AlignCenter()
            .AlignMiddle();

}

void GameplayState::InitTimerBar(){

    //timer Bar
    timeBar = &canvas->AddElement()
            .SetColor({0,1,0,1})
            .SetAbsoluteSize({800, 30})
            .AlignCenter()
            .AlignTop(30);
}

void GameplayState::UpdatePlayerBlip(Element& element, float dt) {
    if (!firstPersonPosition) return;
    float tVal = CalculateCompletion(firstPersonPosition->GetPosition());
    tVal = std::clamp(tVal, 0.0f, 1.0f);
    tVal = tVal * 300 - 150;

    element.AlignMiddle((int)tVal);
}

void GameplayState::InitLevelMap(){
    //map bar
    auto& levelBar = canvas->AddElement()
            .SetColor({1,1,1,1})
            .SetAbsoluteSize({20, 300})
            .AlignRight(20)
            .AlignMiddle();

    auto& playerElement = canvas->AddElement()
            .SetColor({0.0,0.0,0.,1})
            .SetAbsoluteSize({20,20})
            .AlignRight(20)
            .AlignMiddle(-150);

    playerElement.OnUpdate.connect<&GameplayState::UpdatePlayerBlip>(this);
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
    world->StartWorld();
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

    levelLen = (lr->GetEndPosition()-lr->GetStartPosition()).Length();
    startPos = lr->GetStartPosition();
    // TEST SWINGING OBJECT ON THE CLIENT
    auto swingingTemp = new GameObject();
    replicated->AddSwingingBlock(swingingTemp, *world);
    swingingTemp->SetRenderObject(new RenderObject(&swingingTemp->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));

    auto tem = new GameObject();
    replicated->AddTestObjectToLevel(tem, *world, {10,10,10}, {0,10,0});
    tem->SetRenderObject(new RenderObject(&tem->GetTransform(),resources->GetQuadMesh(),nullptr, nullptr));

}

void GameplayState::SetTestSprings() {
    for (int i = 0; i < 4; i++) {
        auto g = new GameObject();
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -40.0f));
        g->SetRenderObject(new RenderObject(&g->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));
        g->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f / 4.0f * i, 0.0f, 1.0f));
    }

    for (int i = 0; i < 4; i++) {
        auto g = new GameObject();
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -50.0f));
        g->SetRenderObject(new RenderObject(&g->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));
        g->GetRenderObject()->SetColour(Vector4(0, 1.0f / 4.0f * i, 1.0f, 1.0f));
    }

}

void GameplayState::SetTestFloor() {
    auto g2 = new GameObject();
    auto x = new PrimitiveGameObject();
    x->position = Vector3(0, -5, 0);
    x->colliderExtents = Vector3(200, 2, 200);
    x->dimensions = Vector3(200, 2, 200);

    replicated->AddBlockToLevel(g2, *world, x);
    g2->SetRenderObject(new RenderObject(&g2->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));
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

float GameplayState::CalculateCompletion(Vector3 playerCurPos){
    auto progress = playerCurPos - startPos;
    return progress.Length()/levelLen;
}