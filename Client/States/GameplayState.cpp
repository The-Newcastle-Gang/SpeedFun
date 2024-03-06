#include "GameplayState.h"
#include "GameplayState.h"
#include "GameplayState.h"

using namespace NCL;
using namespace CSC8503;

# define PI 3.141592f

GameplayState::GameplayState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Resources* pResources, Canvas* pCanvas, SoundManager* pSoundManager) : State() {
    renderer = pRenderer;
    world = pGameworld;
    soundManager = pSoundManager;
    // Don't touch base client in here, need some way to protect this.
    baseClient = pClient;
    resources = pResources;
    replicated = std::make_unique<Replicated>();
    canvas = pCanvas;

    timeBar = new Element(1);

}

GameplayState::~GameplayState() {
    delete testParticles;
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
            .SetColor({1.0,1.0,1.0,1.0})
            .SetAbsoluteSize({15,3})
            .AlignCenter()
            .AlignMiddle();

    auto crossHeirHoriz = canvas->AddElement()
            .SetColor({1.0,1.0,1.0,1.0})
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

void GameplayState::LoadParticleSystems()
{
    testParticles = new ParticleSystem({ 0, 0, 0 }, { -100, 0, -30 }, { 200, 1, 5 }, 25, 10.0f, 10, 1, 0.25f, resources->GetTexture("Default.png"));
    particleSystems.push_back(testParticles);

    


    renderer->PassParticleSystems(particleSystems);
}

void GameplayState::UpdateParticleSystems(float dt)
{
    for (auto ps : particleSystems)
    {
        ps->CreateNewParticles(dt);
        ps->UpdateParticles(dt, world->GetMainCamera()->GetPosition());
    }
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
    InitCanvas();
    InitSounds();

}

void GameplayState::InitSounds() {
    soundManager->AddSoundsToLoad({ "koppen.ogg" , "footsteps.wav" });
    soundManager->LoadSoundList();
    soundManager->SM_PlaySound("koppen.ogg");
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
    soundManager->UnloadSoundList();
    delete networkThread;
}

void GameplayState::Update(float dt) {
    ResetCameraAnimation();
    SendInputData();
    ReadNetworkFunctions();

    Window::GetWindow()->ShowOSPointer(false);
    //Window::GetWindow()->LockMouseToWindow(true);

    if (firstPersonPosition) {
        world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
    }
    WalkCamera(dt);
    if (jumpTimer > 0) JumpCamera(dt);
    if (landTimer > 0) LandCamera(dt);
    StrafeCamera(dt);

    world->GetMainCamera()->UpdateCamera(dt);
    world->UpdateWorld(dt);

    ReadNetworkPackets();

    // particle updates
    UpdateParticleSystems(dt);

    renderer->Render();
    Debug::UpdateRenderables(dt);

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM7)) {
        ResetCameraToForwards();
    }
}

void GameplayState::ResetCameraToForwards() {
    world->GetMainCamera()->SetPitch(0.68f);
    world->GetMainCamera()->SetYaw(269.43f);

}

void GameplayState::ReadNetworkFunctions() {
    while (!networkData->incomingFunctions.IsEmpty()) {
        FunctionPacket packet = networkData->incomingFunctions.Pop();
        DataHandler handler(&packet.data);
        switch (packet.functionId) {
            case(Replicated::AssignPlayer): {
                int networkId = handler.Unpack<int>();
                AssignPlayer(networkId);
            }
            break;

            case(Replicated::Camera_GroundedMove): {
                float intesnity = handler.Unpack<float>();
                groundedMovementSpeed = intesnity;
            }
            break;

            case(Replicated::Camera_Jump): {
                jumpTimer = 3.14f;
            }
            break;
            
            case(Replicated::Camera_Land): {
                float grounded = handler.Unpack<float>();
                landIntensity = std::clamp(grounded, 0.0f, landFallMax);
                landTimer = 3.14f;
            }
            break;
            
            case(Replicated::Camera_Strafe): {
                float strfSpd = handler.Unpack<float>();
                strafeSpeed = strfSpd;
            }
            break;

            case(Replicated::Grapple_Event): {
                int eventType = handler.Unpack<int>();
                HandleGrappleEvent(eventType);
            }
            break;

            case(Replicated::Player_Velocity_Call): {
                Vector3 velocity = handler.Unpack<Vector3>();
                playerVelocity = velocity;
                float speed = std::max(0.0f, velocity.Length() - 8.0f);
                renderer->SetSpeedLineAmount(std::min(speed, 40.0f)/40.0f);
            }
            break;
        }

    }
}
void GameplayState::ResetCameraAnimation() {
    groundedMovementSpeed = groundedMovementSpeed * 0.95f;
    strafeSpeed = 0.0f;
}
void GameplayState::WalkCamera(float dt) {
    world->GetMainCamera()->SetOffsetPosition(Vector3(0, abs(bobFloor + bobAmount *sin(walkTimer)) * (groundedMovementSpeed / maxMoveSpeed), 0));
    
    groundedMovementSpeed = groundedMovementSpeed * 0.95 + currentGroundSpeed * 0.05;
    if (walkSoundTimer <= 0) {
        soundManager->SM_PlaySound("footsteps.wav");
        walkSoundTimer += PI;
    }
    float bobHeight = abs(bobFloor + bobAmount * sin(walkTimer));
    world->GetMainCamera()->SetOffsetPosition(Vector3(0, bobHeight * (groundedMovementSpeed / maxMoveSpeed), 0));
    walkTimer += dt * groundedMovementSpeed * 0.75f;
    walkSoundTimer -= dt * groundedMovementSpeed * 0.75f;
}

void GameplayState::JumpCamera(float dt) {
    world->GetMainCamera()->SetOffsetPosition(world->GetMainCamera()->GetOffsetPosition() + Vector3(0, -jumpBobAmount * sin(PI - jumpTimer), 0));
    jumpTimer = std::clamp(jumpTimer - dt * jumpAnimationSpeed, 0.0f, PI);
}

void GameplayState::HandleGrappleEvent(int event) {
    switch (event) {
        case 1: {
            //renderer->SetSpeedActive(true);
            break;
        }
        case 2: {
            //renderer->SetSpeedActive(false);

           break;
        }
    }
}

void GameplayState::LandCamera(float dt) {
    world->GetMainCamera()->SetOffsetPosition(world->GetMainCamera()->GetOffsetPosition() + 
                                              Vector3(0, -landBobAmount * sin(PI - PI * sin(PI /2 - landTimer/2)) / landFallMax * landIntensity, 0));
    landTimer = std::clamp(landTimer - dt * landAnimationSpeed, 0.0f, PI);
}

void GameplayState::StrafeCamera(float dt) {
    strafeAmount = strafeAmount * 0.95f + strafeSpeed * 0.05f;
    world->GetMainCamera()->SetRoll(-strafeTiltAmount * (strafeAmount / strafeSpeedMax));
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

void GameplayState::CreateRock() {
    auto rock = new GameObject("Rock");
    world->AddGameObject(rock, true);
    auto volume = new AABBVolume(Vector3(1.0, 1.0, 1.0));
    rock->SetBoundingVolume((CollisionVolume*)volume);

    rock->GetTransform()
            .SetScale(Vector3(1.0, 1.0, 1.0))
            .SetPosition(Vector3(0, 20, 0));

    rock->SetRenderObject(new RenderObject(&rock->GetTransform(), resources->GetMesh("stone_tallA.obj"), nullptr, nullptr));
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
    lr->HasReadLevel("newTest.json");
    auto plist  = lr->GetPrimitiveList();
    auto opList  = lr->GetOscillatorPList();
    auto harmOpList  = lr->GetHarmfulOscillatorPList();

    for(auto &x : plist){
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({0.0f, 0.65f, 0.90f, 1.0f});
    }

    for (auto &x : opList) {
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.5f,0.0f, 1.0f });
    }

    for (auto &x : harmOpList) {
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.0f,0.0f, 1.0f });
    }


    //SetTestSprings();

    //SetTestFloor();

    levelLen = (lr->GetEndPosition()-lr->GetStartPosition()).Length();
    startPos = lr->GetStartPosition();
    // TEST SWINGING OBJECT ON THE CLIENT
    auto swingingTemp = new GameObject();
    replicated->AddSwingingBlock(swingingTemp, *world);
    swingingTemp->SetRenderObject(new RenderObject(&swingingTemp->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));

    // load particles
    LoadParticleSystems();

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