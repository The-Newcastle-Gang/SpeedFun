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
    world->ClearAndErase();
    renderer->Render();
    delete networkThread;
}

void GameplayState::Update(float dt) {
    SendInputData();
    ReadNetworkFunctions();

    if (firstPersonPosition) {
        world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
        RaycastEnemy(dt, firstPersonPosition);
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
    CreatePlayers();
    InitLevel();
    AddRaycastEnemy(Vector3(-80, 6, -7));
}

void GameplayState::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);
        player->SetRenderObject(new RenderObject(&player->GetTransform(), resources->GetMesh("Capsule.msh"), nullptr, nullptr));
    }
}

void GameplayState::InitLevel(){

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

void GameplayState::RaycastEnemy(float dt, Transform* playerPos) {
    Vector3 rayPos;
    Vector3 rayDir;
    Vector3 player1Pos;
    Vector3 enemyPos;
    Vector3 enemyDir;
    Quaternion enemyOrientation;

    player1Pos = playerPos->GetPosition();
    enemyPos = raycastEnemy->GetTransform().GetPosition();

    if (player1Pos.x >= enemyPos.x - 10 && player1Pos.x <= enemyPos.x + 10
        && player1Pos.z >= enemyPos.z - 10 && player1Pos.z <= enemyPos.z + 10) {
         warnningTime += dt;
         if (warnningTime >= 5) {
             std::cout << "You are shooted by the enemy" << std::endl;
             warnningTime = 0.0;
         }    // didn't cout the word
         //rayDir = (player1Pos - enemyPos).Normalised();
         //rayPos = raycastEnemy->GetTransform().GetPosition();
         //Debug::DrawLine(rayPos, player1Pos, Vector4(1, 0, 0, 1));//someting problem on it   wrong phenomenon

         enemyDir = (enemyPos - player1Pos).Normalised();
         float transDegree = (atan2(enemyDir.x, enemyDir.z)) * 180.0f / 3.14159265358979323846f;
         enemyOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, transDegree, 0.0f);
         raycastEnemy->GetTransform().SetOrientation(enemyOrientation);
     }
 }

 GameObject* GameplayState::AddRaycastEnemy(const Vector3& position) {
     float meshSize = 0.5f;
     raycastEnemy->GetTransform()
         .SetScale(Vector3(meshSize, meshSize, meshSize))
         .SetPosition(position);
     raycastEnemy->SetRenderObject(new RenderObject(&raycastEnemy->GetTransform(), resources->GetMesh("goose.msh"), nullptr, nullptr));
     world->AddGameObject(raycastEnemy, true);

     return raycastEnemy;
 }