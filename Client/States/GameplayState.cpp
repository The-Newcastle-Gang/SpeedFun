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
    loadingImage = new Element(1);
    loadingText = new Element(1);
    levelManager = std::make_unique<LevelManager>();
    medalImage = "medal.png";
    titleShader = renderer->LoadShader("defaultUI.vert", "fireUI.frag");
    menuFont = renderer->LoadFont("Ubuntu-Bold.ttf", 55);
}

GameplayState::~GameplayState() {

    shouldShutDown.store(true);
    networkThread->join();

    delete networkThread;
    networkThread = nullptr;

    delete debugger;
    delete loadSoundThread;
}


void GameplayState::InitCanvas(){

    //this is clearly not the best way to do the cross-heir but This will have to do for now
    //since i wanna just use this as debug.
    //I can bet money on the fact that this code is going to be at release
    //if u see this owen dont kill this

    shouldLoadScreen.store(true);
    if (finishedLoading == LoadingStates::READY) {
    }
    canvas->Reset();
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
            .AlignMiddle(-150)
            .AlignMiddle(-150);

    playerElement.OnUpdate.connect<&GameplayState::UpdatePlayerBlip>(this);
}

void GameplayState::ThreadUpdate(GameClient* client, ClientNetworkData* networkData) {

    auto threadClient = ClientThread(client, networkData);

    while (!shouldShutDown) {
        threadClient.Update();
    }
}

void GameplayState::LoadingScreenTUpdate() {

    int &&i = 10;
    while (!shouldLoadScreen) {
        std::cout << "Loading!\n";
        i += 10;
        //&loadingImage->SetColor({0.0f,0.0f,1.0f,1.0f});
        renderer->RenderLoadingScreen();
        //loadingImage.set pos

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void GameplayState::OnEnter() {
    firstPersonPosition = nullptr;
    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);
    CreateLoadingScreenThread();
    CreateNetworkThread();
    debugger = new DebugMode(world->GetMainCamera());
    InitialiseAssets();
}
void GameplayState::InitialiseAssets() {
    
    soundManager->SM_AddSongsToLoad({ "goodegg.ogg", "koppen.ogg", "neon.ogg", "scouttf2.ogg", "skeleton.ogg" });
    std::string songToPlay = soundManager->SM_SelectRandomSong();
    soundsToLoad.push_back(songToPlay);

    totalThingsToLoad = soundsToLoad.size();
    InitWorld();
    InitCamera();
    

    loadSoundThread = new std::thread(&GameplayState::InitSounds, this);
    loadSoundThread->detach();
    /*FinishLoading();*/
}
void GameplayState::InitSounds() {
    // Believe this could be thread unsafe, as sounds can be accessed while this theoretically is still loading, with no
    // guards on the PlaySound method.

    soundManager->SM_AddSongsToLoad({ "goodegg.ogg", "koppen.ogg", "neon.ogg", "scouttf2.ogg", "skeleton.ogg", "peakGO.ogg" });

    std::string songToPlay = soundManager->SM_SelectRandomSong();
    soundManager->SM_AddSoundsToLoad({ songToPlay, "footsteps.wav", "weird.wav" , "warning.wav", "Death_sound.wav" });
    soundManager->SM_LoadSoundList();

    // Probably should be made atomic though probably doesn't matter.
    soundHasLoaded = LoadingStates::LOADED;
}

void GameplayState::CreateNetworkThread() {
    shouldShutDown.store(false);
    GameClient* client = baseClient;
    networkData = std::make_unique<ClientNetworkData>();
    networkThread = new std::thread(&GameplayState::ThreadUpdate, this, client, networkData.get());
}

void GameplayState::CreateLoadingScreenThread() {
    canvas->Reset();

    //InitLua();
    //lua_getglobal(L, "canvas");
    //lua_pushnil(L);
    //while (lua_next(L, -2)) {
    //    auto layerName = lua_tostring(L, -2);
    //    lua_pushnil(L);
    //    while (lua_next(L, -2)) {
    //        AddCanvasElement(layerName, false);
    //        lua_pop(L, 1);
    //    }
    //    lua_pop(L, 1);
    //}
    
    canvas->CreateNewLayer("Loading");
    canvas->PushActiveLayer("Loading");
    canvas->AddImageElement("LoadingText.png", "Loading").SetAbsoluteSize({ 500,500 }).AlignTop().AlignCenter();
    loadingImage = &canvas->AddImageElement("Circle.png", "Loading").SetAbsoluteSize({ 100,100 }).AlignMiddle().AlignRight(100);
    renderer->RenderLoadingScreen();

    canvas->CreateNewLayer("Loading2");
    canvas->PushActiveLayer("Loading2");
    canvas->AddImageElement("LoadingText.png", "Loading2").SetAbsoluteSize({ 500,500 }).AlignTop().AlignCenter();
    canvas->AddImageElement("Circle.png", "Loading2").SetAbsoluteSize({ 100,100 }).AlignMiddle().AlignLeft(500);
    renderer->RenderLoadingScreen();
    &loadingImage->SetRelativePosition({100,500});

    shouldLoadScreen.store(false);
    loadingScreenThread = new std::thread(&GameplayState::LoadingScreenTUpdate, this);
}

void GameplayState::AddCanvasElement(const std::string& layerName, bool blocking) {

    if (layerName != "main" && !canvas->DoesLayerExist(layerName)) {
        canvas->CreateNewLayer(layerName, blocking);
    }

    std::string image = getStringField(L, "image");

    auto& element = image.empty() ? canvas->AddElement(layerName)
        : canvas->AddImageElement(image, layerName);

    element.SetAbsoluteSize(getVec2Field(L, "aSize"))
        .SetRelativeSize(getVec2Field(L, "rSize"))
        .SetAbsolutePosition(getVec2Field(L, "aPos"))
        .SetRelativePosition(getVec2Field(L, "rPos"))
        .SetColor(getVec4Field(L, "color"))
        .SetZIndex(getIntField(L, "zIndex"));

    AlignCanvasElement(element);

    TextData text;
    getTableField(L, "text");

    text.text = getStringField(L, "text");
    text.defaultText = text.text;
    text.color = getVec4Field(L, "color");
    text.SetFont(menuFont.get());
    text.fontSize = (float)getNumberField(L, "size");

    element.SetText(text);
    lua_pop(L, 1); // text

    std::unordered_set<std::string> tags;
    getTableField(L, "tags");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        tags.insert(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // tags

    std::string id = getStringField(L, "id");

    element.AddTags(tags);
    element.SetId(id);
}

void GameplayState::AlignCanvasElement(Element& element) {
    getTableField(L, "align");
    lua_pushnil(L);

    while (lua_next(L, -2)) {
        auto direction = getStringField(L, "to");
        auto padding = getIntField(L, "padding");
        if (!strcmp(direction, "top")) {
            element.AlignTop(padding);
        }
        else if (!strcmp(direction, "middle")) {
            element.AlignMiddle(padding);
        }
        else if (!strcmp(direction, "bottom")) {
            element.AlignBottom(padding);
        }
        else if (!strcmp(direction, "left")) {
            element.AlignLeft(padding);
        }
        else if (!strcmp(direction, "center")) {
            element.AlignCenter(padding);
        }
        else if (!strcmp(direction, "right")) {
            element.AlignRight(padding);
        }
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // align
}

void GameplayState::InitLua() {
    L = luaL_newstate();
    luaL_openlibs(L);

    auto status = luaL_dofile(L, (Assets::DATADIR + "LoadingScreen.lua").c_str());

    if (status) {
        std::cerr << "LUA CRIES OUT IN PAIN: " << lua_tostring(L, -1);
    }
}



void GameplayState::OnExit() {
    Window::GetWindow()->LockMouseToWindow(false);
    Window::GetWindow()->ShowOSPointer(true);

    world->ClearAndErase();
    renderer->Render();
    soundManager->SM_UnloadSoundList();
    baseClient->ClearPacketHandlers();

    shouldShutDown.store(true);
    networkThread->join();
    
    delete networkThread;
    networkThread = nullptr;
    lua_close(L);
}

void GameplayState::ManageLoading(float dt) {

    if (loadingTime > 0.1f) {
        //std::cout << ".\n";
        loadingTime = 0.0f;
    }
    loadingTime += dt;


    if (soundHasLoaded == LoadingStates::LOADED) {
        std::cout << "\n\nSounds Have Loaded!\n\n";
        soundManager->SM_PlaySound(soundManager->GetCurrentSong());
        soundHasLoaded = LoadingStates::READY;
    }
    
    if (worldHasLoaded == LoadingStates::LOADED) {
        worldHasLoaded = LoadingStates::READY;
    }

    if (soundHasLoaded == LoadingStates::READY && worldHasLoaded == LoadingStates::READY) {
        delete loadSoundThread;
        finishedLoading = LoadingStates::READY;
        FinishLoading();
    }
}

void GameplayState::Update(float dt) {
    ResetCameraAnimation();
    if (finishedLoading != LoadingStates::READY) {
        ManageLoading(dt);
        return;
    }

    totalDTElapsed += dt;
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

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) displayDebugger = !displayDebugger;
    if (displayDebugger) debugger->UpdateDebugMode(dt);
    if (debugMovementEnabled) {
        // idk i got bored
        for (int i = 0; i < 6; i++) {
            Debug::Print("Debug Movement!", 
                Vector2(2.0f + 0.5f * cos(2.0f * PI / 6.0f * i + totalDTElapsed), 94.0f + 0.5f * sin(2.0f * PI / 6.0f * i + totalDTElapsed)),
                Debug::RAINBOW_ARRAY[i]);
        }
        for (int i = 0; i < 6; i++) {
            Debug::Print("Debug Movement!",
                Vector2(2.0f + 0.25f * cos(2.0f * PI / 6.0f * i + totalDTElapsed), 94.0f + 0.25f * sin(2.0f * PI / 6.0f * i + totalDTElapsed)),
                Debug::BLACK);
        }
        Debug::Print("Debug Movement!", Vector2(2, 94), Debug::WHITE);
    }
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
            } break;

            case(Replicated::Camera_GroundedMove): {
                float intesnity = handler.Unpack<float>();
                currentGroundSpeed = intesnity;
            } break;

            case(Replicated::Camera_Jump): {
                jumpTimer = PI;
            } break;

            case(Replicated::Camera_Land): {
                float grounded = handler.Unpack<float>();
                landIntensity = std::clamp(grounded, 0.0f, landFallMax);
                landTimer = PI;
            } break;

            case(Replicated::Camera_Strafe): {
                float strfSpd = handler.Unpack<float>();
                strafeSpeed = strfSpd;
            } break;

            case(Replicated::Stage_Start): {
                // Enable player controls

            } break;

            case(Replicated::EndReached): {
                int networkId = handler.Unpack<int>();
                int medal = handler.Unpack<int>();
                Vector4 medalColour = handler.Unpack<Maths::Vector4>();

                canvas->CreateNewLayer("FinishedLevelLayer");
                canvas->PushActiveLayer("FinishedLevelLayer");

                canvas->AddImageElement(GetMedalImage(), "FinishedLevelLayer")
                        .SetColor(medalColour)
                        .SetAbsoluteSize({60,60})
                        .AlignCenter()
                        .AlignLeft();
                // Disable player controls
                // Clear the world
                // Loading screen
                // Load the next level

            } break;

            case(Replicated::Death_Event): {
                //
                // Play Anim
                soundManager->SM_PlaySound("Death_sound.wav");
                ResetCameraToForwards();

            } break;

            case(Replicated::Death_Event_End): {
                // Function set up for later use.
                    //canvas->PopActiveLayer();
            } break;

            case(Replicated::Grapple_Event): {
                int eventType = handler.Unpack<int>();
                HandleGrappleEvent(eventType);
            } break;


            case(Replicated::Player_Velocity_Call): {
                Vector3 velocity = handler.Unpack<Vector3>();
                playerVelocity = velocity;
                float speed = std::max(0.0f, velocity.Length() - 10.0f);
                float speedVisualModifier = std::min(speed, 50.0f) / 50.0f;
                renderer->SetSpeedLineAmount(speedVisualModifier);
                world->GetMainCamera()->SetFieldOfVision( defaultFOV + speedVisualModifier * 20.0f);
            }
            break;

            case(Replicated::Player_Animation_Call): {
                Replicated::RemoteAnimationData data = handler.Unpack< Replicated::RemoteAnimationData>();
                UpdatePlayerAnimation(data.networkID, data.state);
            }
        }
    }
}

void GameplayState::UpdatePlayerAnimation(int networkID, Replicated::PlayerAnimationStates state) {
    GameObject *playerObject = world->GetObjectByNetworkId(networkID);
    AnimatorObject *playerAnimator = playerObject->GetAnimatorObject();
    if (!playerAnimator)return;

    switch (state) {
        case Replicated::IDLE: {
            playerAnimator->TransitionAnimation("Idle", 0.1f);
            break;
        }
        case Replicated::JUMP: {
            playerAnimator->TransitionAnimation("Jump", 0.1f);
            break;
        }
        case Replicated::FALLING: {
            playerAnimator->TransitionAnimation("Fall", 0.1f);
            break;
        }
        case Replicated::RUNNING_FORWARD: {
            playerAnimator->TransitionAnimation("Run", 0.1f);
            break;
        }
        case Replicated::RUNNING_BACK: {
            playerAnimator->TransitionAnimation("RunBack", 0.1f);
            break;
        }
        case Replicated::RUNNING_LEFT: {
            playerAnimator->TransitionAnimationWithMidPose("LeftStrafe", 0.15f);
            break;
        }
        case Replicated::RUNNING_RIGHT: {
            playerAnimator->TransitionAnimationWithMidPose("RightStrafe", 0.15f);
            break;
        }
    }
}

std::string GameplayState::GetMedalImage(){
    return medalImage;

}

void GameplayState::ResetCameraAnimation() {
    currentGroundSpeed = 0.0f;
    strafeSpeed = 0.0f;
}

void GameplayState::WalkCamera(float dt) {
    
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
            isGrappling = true;
            break;
        }
        case 2: {
            isGrappling = false;
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

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O)) {
        (*networkData).outgoingFunctions.Push(FunctionPacket(Replicated::RemoteServerCalls::PlayerDebug, nullptr));
        debugMovementEnabled = !debugMovementEnabled;
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

void GameplayState::FinishLoading() {
    InitCanvas();
    networkData->outgoingFunctions.Push(FunctionPacket(Replicated::GameLoaded, nullptr));
    world->StartWorld();
}

void GameplayState::InitCamera() {
    Camera* cam = world->GetMainCamera();
    cam->SetNearPlane(0.1f);
    cam->SetFarPlane(500.0f);
    cam->SetPitch(-15.0f);
    cam->SetYaw(315.0f);
    cam->SetPosition(Vector3(-60, 40, 60));
    cam->SetCameraOffset(Vector3(0, 0.5f,0 )); //to get the camera to the player's head
}

void GameplayState::InitWorld() {
    InitLevel();
    CreatePlayers();
    worldHasLoaded = LoadingStates::LOADED;
}

[[maybe_unused]]
void GameplayState::CreateRock() {
    auto rock = new GameObject("Rock");
    world->AddGameObject(rock, true);
    auto volume = new AABBVolume(Vector3(1.0, 1.0, 1.0));
    rock->SetBoundingVolume((CollisionVolume*)volume);

    rock->GetTransform()
            .SetScale(Vector3(1.0, 1.0, 1.0))
            .SetPosition(Vector3(-50, 5, 0));

    rock->SetRenderObject(new RenderObject(&rock->GetTransform(), resources->GetMesh("trident.obj"), resources->GetTexture("FlatColors.png"), nullptr));
}

void GameplayState::CreatePlayers() {
    OGLShader* playerShader = new OGLShader("SkinningVert.vert", "Player.frag");
    MeshGeometry* playerMesh = resources->GetMesh("Player.msh");
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);

        playerMesh->AddAnimationToMesh("Run", resources->GetAnimation("Player_FastRun.anm"));
        playerMesh->AddAnimationToMesh("LeftStrafe", resources->GetAnimation("Player_RightStrafe.anm")); //this is just how the animations were exported
        playerMesh->AddAnimationToMesh("RightStrafe", resources->GetAnimation("Player_LeftStrafe.anm"));
        playerMesh->AddAnimationToMesh("RunBack", resources->GetAnimation("Player_RunBack.anm"));
        playerMesh->AddAnimationToMesh("Idle", resources->GetAnimation("Player_Idle.anm"));
        playerMesh->AddAnimationToMesh("Fall", resources->GetAnimation("Player_Fall.anm"));
        playerMesh->AddAnimationToMesh("Jump", resources->GetAnimation("Player_Grapple.anm"));
        player->SetRenderObject(new RenderObject(&player->GetTransform(), playerMesh, nullptr, playerShader));
        player->GetRenderObject()->SetMeshScale(player->GetTransform().GetScale() * 0.6f);
        player->GetRenderObject()->SetMeshOffset(Vector3(0,-0.2f,0));

        AnimatorObject* newAnimator = new AnimatorObject(playerMesh->GetAnimationMap());
        newAnimator->SetAnimation(playerMesh->GetAnimation("Idle"));
        newAnimator->SetMidPose("Idle");
        player->SetAnimatorObject(newAnimator);
        player->GetRenderObject()->SetAnimatorObject(newAnimator);
        player->GetRenderObject()->SetMeshMaterial(resources->GetMeshMaterial("Player.mat"));

    }
}

void GameplayState::InitLevel() {
    levelManager->TryReadLevel("newTest");

    auto plist  = levelManager->GetLevelReader()->GetPrimitiveList();
    auto opList  = levelManager->GetLevelReader()->GetOscillatorPList();
    auto harmOpList  = levelManager->GetLevelReader()->GetHarmfulOscillatorPList();

    totalThingsToLoad += plist.size() + opList.size() + harmOpList.size();
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

    levelLen = (levelManager->GetLevelReader()->GetEndPosition() - levelManager->GetLevelReader()->GetStartPosition()).Length();
    startPos = levelManager->GetLevelReader()->GetStartPosition();

    // TEST SWINGING OBJECT ON THE CLIENT
    auto swingingTemp = new GameObject();
    replicated->AddSwingingBlock(swingingTemp, *world);
    swingingTemp->SetRenderObject(new RenderObject(&swingingTemp->GetTransform(), resources->GetMesh("Sphere.msh"), nullptr, nullptr));
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

    delete player->GetRenderObject();
    player->SetRenderObject(nullptr);

    delete player->GetAnimatorObject();
    player->SetAnimatorObject(nullptr);

    firstPersonPosition = &player->GetTransform();
    std::cout << "Assigning player to network object: " << player->GetNetworkObject()->GetNetworkId() << std::endl;
}

float GameplayState::CalculateCompletion(Vector3 playerCurPos){
    auto progress = playerCurPos - startPos;
    return progress.Length()/levelLen;
}