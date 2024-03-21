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
    levelManager = std::make_unique<LevelManager>();
    medalImage = "medal.png";
    crosshairImage = "crosshair.png";
    LoadPlayerMeshMaterials();
    playerblipImage = "playerBlip.png";

    timerBarShader      = resources->GetShader("timerBar");
    fireShader          = renderer->LoadShader("defaultUI.vert", "fireTimer.frag");
    fireShader          = renderer->LoadShader("defaultUI.vert", "fireTimer.frag");
    medalShineShader    = renderer->LoadShader("defaultUI.vert", "medalShine.frag");
    biggerDebugFont     = std::unique_ptr(renderer->LoadFont("CascadiaMono.ttf", 48 * 3));
    soundEffects = { "footsteps.wav", "weird.wav" , "warning.wav", "Death_sound.wav" , 
        "sfx_chainhook.wav",
        "sfx_chainready.wav",
        "sfx_chainreel.wav",
        "sfx_chainthrow.wav",
        "sfx_walk.wav",
        "sfx_walk2.wav",
        "sfx_timetally.wav",
        "sfx_timeshake.wav",
        "sfx_medal.wav",
        "sfx_countdown.wav",
        "sfx_go.wav"};
    walkSounds = { "sfx_walk2.wav" };
}

GameplayState::~GameplayState() {
    delete lavaParticles;
    particleSystems.clear();

    shouldShutDown.store(true);
    networkThread->join();

    delete networkThread;
    networkThread = nullptr;

    delete loadSoundThread;
    loadSoundThread = nullptr;

    delete medalShineShader;
    medalShineShader = nullptr;

    delete cinematicCamera;
}


void GameplayState::InitCanvas(){
    shouldLoadScreen.store(false);
    canvas->Reset();
    
    InitStartScreen();
    InitEndCanvas();
    InitCrossHeir();
    InitTimerBar();
    InitLevelMap();
    InitPauseScreen();
}

void GameplayState::InitStartScreen() {
    canvas->CreateNewLayer("StartScreenLayer");
    
    auto startBack = canvas->AddElement("StartScreenLayer")
        .SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f))
        .SetAbsoluteSize({ 2000,2000 })
        .AlignCenter()
        .AlignMiddle()
        .SetId("StartScreen_Back");
    startBack.OnUpdate.connect<&GameplayState::UpdateStartBack>(this);

    TextData textData;
    textData.font = biggerDebugFont.get();
    auto countdownText = canvas->AddElement("StartScreenLayer")
        .SetColor({ 1,1,1,0 })
        .SetAbsoluteSize({ 80, 100 })
        .AlignCenter()
        .AlignMiddle(-40)
        .SetId("StartScreen_Countdown")
        .SetText(textData);
    countdownText.OnUpdate.connect<&GameplayState::UpdateStartText>(this);

    textData.fontSize = 0.5f;
    auto levelNameText = canvas->AddElement("StartScreenLayer")
        .SetColor({ 1,1,1,0 })
        .SetAbsoluteSize({ 0, 50 })
        .AlignCenter()
        .AlignMiddle()
        .SetId("StartScreen_LevelName")
        .SetText(textData);
    levelNameText.OnUpdate.connect<&GameplayState::UpdateStartText>(this);

    

}

void GameplayState::InitEndCanvas() {
    canvas->CreateNewLayer("FinishedLevelLayer");

    canvas->AddElement("FinishedLevelLayer")
        .SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.75f))
        .SetAbsoluteSize({ 2000,2000 })
        .AlignCenter()
        .AlignMiddle();

    auto endMedalElement = canvas->AddImageElement(GetMedalImage(), "FinishedLevelLayer")
        .SetColor(Vector4(0, 0, 0, 0))
        .SetAbsoluteSize({ 320,320 })
        .CenterSprite()
        .SetTransformTranslation(Vector2(25, 50))
        .SetShader(medalShineShader);

    endMedalElement.OnUpdate.connect<&GameplayState::UpdateMedalSprite>(this);
    endMedalElementIndex = endMedalElement.GetIndex();


    TextData textData;
    textData.font = biggerDebugFont.get();
    auto finalTime = canvas->AddElement("FinishedLevelLayer")
        .SetColor({ 1,1,1,1 })
        .CenterSprite()
        .SetText(textData);
    //.textData.font = biggerDebugFont.get();


    finalTime.OnUpdate.connect<&GameplayState::UpdateFinalTimeTally>(this);
}

void GameplayState::ClearLevel()
{
    world->ClearAndErase();
}

void GameplayState::InitCrossHeir(){
    //crossheir
    crosshair = &canvas->AddImageElement(crosshairImage)
        .SetAbsoluteSize({ 32, 32 })
        .CenterSprite();
    crosshair->OnUpdate.connect<&GameplayState::UpdateCrosshair>(this);
}

void GameplayState::InitTimerBar(){
    //timer Bar
    if (isSinglePlayer)
    {
        Vector4 colours[3] = { Replicated::GOLD, Replicated::SILVER, Replicated::BRONZE };
        for (int i = 0; i < 3; i++) {

            timerNubs[i] = &canvas->AddElement()
                .SetColor(colours[i])
                .SetAbsoluteSize({ 4, timerBarHeight + 22 })
                .AlignTop(timerTopOffset - 11)
                .SetId("nub_" + std::to_string(i));

            medalTimeRatios.insert({ timerNubs[i]->GetId(), {i, -1.0f} });
            timerNubs[i]->OnUpdate.connect<&GameplayState::UpdateTimerNub>(this);
        }

        auto platNub = canvas->AddElement()
            .SetColor(Replicated::PLATINUM)
            .SetAbsoluteSize({ 4, timerBarHeight + 22 })
            .AlignCenter(-400 + (800 - timerBoxWidth) + timerBarOutline - 1)
            .AlignTop(timerTopOffset - 11);
    }
    int backTimerBarWidth = (isSinglePlayer) ? 800 : timerBoxWidth;

    auto timerOutline = canvas->AddElement()
        .SetColor({ 0,0,0,1 })
        .SetAbsoluteSize({ backTimerBarWidth + timerBarOutline*2, timerBarHeight + timerBarOutline*2 })
        .AlignCenter()
        .AlignTop(timerTopOffset - timerBarOutline);

    if (isSinglePlayer)
    {
        timeBar = &canvas->AddElement()
            .SetColor(Replicated::PLATINUM)
            .SetAbsoluteSize({ 800, timerBarHeight })
            .AlignCenter()
            .AlignTop(timerTopOffset)
            .SetShader(timerBarShader);
        timeBar->OnUpdate.connect<&GameplayState::UpdateTimerUI>(this);
    }

    //this is the box behind the timer number
    timeBarTimerBox = &canvas->AddElement()
            .SetColor({1,1,1,1})
            .SetAbsoluteSize({ timerEndBoxX , timerEndBoxY})
            .AlignCenter(isSinglePlayer ? 400 : 0)
            .SetShader(fireShader)
            .SetTexture(resources->GetTexture("firemask.jpg"))
            .AlignTop(timerTopOffset - timerEndBoxY* timerEndBoxYoff);

    
    timeBarTimerBox->OnUpdate.connect<&GameplayState::UpdateTimerBox>(this);
    
    
    
    //this is the timer text me thinks
    timerText = &canvas->AddElement()
        .SetColor({ 1,1,1,1 })
        .AlignCenter(400)
        .AlignTop(timerTopOffset - 8)
        .SetText(TextData());
    timerText->OnUpdate.connect<&GameplayState::UpdateTimerText>(this);

}

void GameplayState::LoadParticleSystems()
{
    lavaParticles = new ParticleSystem(deathPos, { -150, 0, -100 }, { 300, 1, 100 }, 50, 10.0f, 3, 0.25f, 0.25f, resources->GetTexture("ember.png"));
    particleSystems.push_back(lavaParticles);

    


    renderer->PassParticleSystems(particleSystems);
}

void GameplayState::UpdateParticleSystems(float dt)
{
    for (auto &ps : particleSystems)
    {
        ps->CreateNewParticles(dt);
        ps->UpdateParticles(dt, world->GetMainCamera()->GetPosition());
    }
}

void GameplayState::UpdatePlayerBlip(Element& element, float dt) {
    if (!firstPersonPosition) return;
    float tVal = CalculateCompletion(playerPositions[element.GetId()]);
    tVal = std::clamp(tVal, 0.0f, 1.0f);
    tVal = tVal * 300 - 150;

    element.GetTransform().SetPosition(Vector3(round(Window::GetWindow()->GetScreenSize().x * 0.98f), round(Window::GetWindow()->GetScreenSize().y / 2 + tVal),0));
}

void GameplayState::InitLevelMap(){
    //map bar

    auto& levelBar = canvas->AddElement()
            .SetColor({0,0,0,1})
            .SetAbsoluteSize({6, 300})
            .CenterSprite()
            .SetTransformTranslation(Vector2(98,50));
    

    /*
    auto& playerElement = canvas->AddImageElement(playerblipImage)
            .SetColor({0.5,0.0,0.,1})
            .SetAbsoluteSize({30,30})
            .CenterSprite();

    playerElement.OnUpdate.connect<&GameplayState::UpdatePlayerBlip>(this);
    */
}

void GameplayState::InitPauseScreen() {
    canvas->CreateNewLayer("PauseLayer");
    
    auto backing = canvas->AddElement("PauseLayer")
        .SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.75f))
        .SetAbsoluteSize({ 2000,2000 })
        .AlignCenter()
        .AlignMiddle();

    TextData textData;
    textData.font = biggerDebugFont.get();
    textData.fontSize = 1.0f;
    textData.text = "RESUME";
    textData.color = { 0.5,0.5,0.5,1 };
    auto resumeText = canvas->AddElement("PauseLayer")
        .SetColor({ 1,1,1,0 })
        .SetAbsoluteSize({ 500,100})
        .AlignCenter()
        .AlignMiddle(75)
        .SetId("Resume")
        .SetText(textData);

    resumeText.OnMouseEnter.connect<&GameplayState::OnPauseHoverEnter>(this);
    resumeText.OnMouseExit.connect<&GameplayState::OnPauseHoverExit>(this);
    resumeText.OnMouseUp.connect<&GameplayState::OnPauseClick>(this);

    textData.text = "EXIT";
    auto exitText = canvas->AddElement("PauseLayer")
        .SetColor({ 1,1,1,0 })
        .SetAbsoluteSize({ 350,100 })
        .AlignCenter()
        .AlignMiddle(-75)
        .SetId("Exit")
        .SetText(textData);
    exitText.OnMouseEnter.connect<&GameplayState::OnPauseHoverEnter>(this);
    exitText.OnMouseExit.connect<&GameplayState::OnPauseHoverExit>(this);
    exitText.OnMouseUp.connect<&GameplayState::OnPauseClick>(this);
    //resumeText.OnMouseEnter.connect<&GameplayState::OnPauseHoverSelect>(this);

    for (int i = 0; i < 2; i++)
    {
        auto& playerElement = canvas->AddImageElement(playerblipImage, "PauseLayer")
            .SetColor({ 0.5,0.0,0.,1 })
            .SetAbsoluteSize({ 125,125 })
            .AlignCenter()
            .AlignMiddle()
            .SetTexture(resources->GetTexture("firemask.jpg"))
            .SetShader(fireShader)
            .SetId("pauseflame_" + std::to_string(i));

        playerElement.OnUpdate.connect<&GameplayState::UpdatePauseFlame>(this);
    }
}

void GameplayState::InitPlayerBlip(int id) {
    auto& playerElement = canvas->AddImageElement(playerblipImage)
        .SetColor({ 0.5,0.0,0.,1 })
        .SetAbsoluteSize({ 60,60 })
        .CenterSprite()
        .SetTexture(resources->GetTexture("firemask.jpg"))
        .SetShader(fireShader)
        .SetId("blip_" + std::to_string(id));

    playerElement.OnUpdate.connect<&GameplayState::UpdatePlayerBlip>(this);
}

void GameplayState::ThreadUpdate(GameClient* client, ClientNetworkData* networkData) {

    auto threadClient = ClientThread(client, networkData);

    while (!shouldShutDown) {
        threadClient.Update();
    }
}

void GameplayState::OnEnter() {
    DebugMode::StartCostClock();

    renderer->SetDeferred(true);
    firstPersonPosition = nullptr;
    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);
    isSinglePlayer = baseClient->IsSinglePlayer();
    CreateLoadingScreenThread();
    CreateNetworkThread();
    InitialiseAssets();
    DebugMode::SetDebugCam(world->GetMainCamera());
    DebugMode::InitDebugInfo();

    WaitForServerLevel(); //wait until server tells us what level to load

    renderer->SetPointLightMesh(resources->GetMesh("Sphere.msh"));
    DebugMode::EndCostClock(0);
}

void GameplayState::WaitForServerLevel() {
    while (!levelManager->GetHasReceivedLevel()) {
        ReadNetworkFunctions();
        //wait till the server tells us what level to load
    }
}

void GameplayState::OnNewLevel() {
    firstPersonPosition = nullptr;
    hasReachedEnd = false;
    displayDebugger = false;
    playerPositions.clear();
    canvas->PopActiveLayer(); //pop end of level UI
    ResetMedalRatios();
    renderer->ClearActiveObjects();
    grapples.clear();
    world->ClearAndErase();
    networkData->incomingState.Clear();
    particleSystems.clear();
    levelManager->Reset();
    finishedLoading = LoadingStates::NOT_LOADED;
    worldHasLoaded = LoadingStates::NOT_LOADED;
    InitCurrentLevel();
    FinishLoading();
    ResetCameraToForwards();

}

void GameplayState::ResetMedalRatios() {
    for (auto& pair : medalTimeRatios) {
        pair.second.second = -1.0f;
    }
}

void GameplayState::InitialiseAssets() {
    loadSoundThread = new std::thread(&GameplayState::InitSounds, this);
    loadSoundThread->detach();
}

void GameplayState::InitCurrentLevel() {
    LoadParticleSystems();
    InitWorld();
    InitCamera();
}

void GameplayState::InitSounds() {
    // Believe this could be thread unsafe, as sounds can be accessed while this theoretically is still loading, with no
    // guards on the PlaySound method.

    soundManager->SM_AddSongsToLoad({
        "the-final-boss-battle-158700.wav",
        "boss-fight-143121.wav",
        "a-dark-desolate-world-15695.wav",
        "the-dying-110458.wav"
        });

    std::string songToPlay = soundManager->SM_SelectRandomSong();
    soundEffects.push_back(songToPlay);
    soundManager->SM_AddSoundsToLoad(soundEffects);
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

    CreateLoadingScreenCanvas();

    shouldLoadScreen.store(true);
    loadingScreenThread = new std::thread(&GameplayState::LoadingScreenUpdate, this);
    loadingScreenThread->detach();
}

void GameplayState::CreateLoadingScreenCanvas() {
    const int loadingScreens = 2;
    Vector2Int lsTextRes = { 742, 42}; // Resolution of actual images
    Vector2Int lsTipTextRes = { 625, 28};
    int textBottomPadding = 35;
    int textLeftPadding = 35;
    int imagePadding = 75;

    for (int i = 1; i <= loadingScreens; i++) {
        std::string currentlayer = "LoadingScreen" + std::to_string(i);
        canvas->CreateNewLayer(currentlayer, false);
        canvas->PushActiveLayer(currentlayer);
        canvas->AddElement(currentlayer)
            .AlignBottom()
            .AlignLeft()
            .SetRelativeSize({ 1,1 })
            .SetColor({ 0,0,0,1 });
        canvas->AddElement(currentlayer)
            .SetAbsoluteSize({ 920,470 })
            .AlignMiddle(imagePadding)
            .AlignCenter();
        canvas->AddImageElement("ScreenShot.png", currentlayer)
                .SetAbsoluteSize({ 904,454 })
                .AlignMiddle(imagePadding)
                .AlignCenter();
        canvas->AddImageElement("TipText.png", currentlayer).AlignLeft(textLeftPadding * 8).AlignBottom(textBottomPadding * 4).SetAbsoluteSize(lsTipTextRes);

        switch (i) {
        case 1:
            canvas->AddImageElement("LS_Text1.png", currentlayer).AlignLeft(textLeftPadding).AlignBottom(textBottomPadding).SetAbsoluteSize(lsTextRes);
            break;
        case 2:
            canvas->AddImageElement("LS_Text2.png", currentlayer).AlignLeft(textLeftPadding).AlignBottom(textBottomPadding).SetAbsoluteSize(lsTextRes);
            break;
        default:
            break;
        }
        renderer->RenderLoadingScreen();
    }
}

void GameplayState::LoadingScreenUpdate() {
    while (shouldLoadScreen) {
        renderer->RenderLoadingScreen();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
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

    delete loadingScreenThread;
    loadingScreenThread = nullptr;
}

void GameplayState::ManageLoading(float dt) {

    if (loadingTime > 0.1f) {
        loadingTime = 0.0f;
    }
    loadingTime += dt;

    if (soundHasLoaded == LoadingStates::LOADED) {
        soundHasLoaded = LoadingStates::READY;
    }

    if (soundHasLoaded == LoadingStates::READY) {
        if (loadSoundThread) {
            delete loadSoundThread;
            loadSoundThread = nullptr;
        }
        finishedLoading = LoadingStates::READY;
    }
}

void GameplayState::Update(float dt) {
    switch (state) {
        case GameplayStateEnums::END_OF_LEVEL:{
            UpdateEndOfLevel(dt);
            break;
        }

        case GameplayStateEnums::PLAYING: {
            UpdatePlaying(dt);
            break;
        }

        case GameplayStateEnums::COUNTDOWN:{
            UpdateCountdown(dt);
            if(dt<1.0f) cinematicCamera->UpdateTimer(dt);
            break;
        }

        case GameplayStateEnums::PLAYER_COMPLETED: {
            UpdatePlayerCompleted(dt);
            break;
        }

    }
}

void GameplayState::UpdateCountdown(float dt){

    if (finishedLoading != LoadingStates::READY) {
        ManageLoading(dt);
        return;
    }
    UpdateAndRenderWorld(dt);


    bool countdownOver = levelManager->UpdateCountdown(dt);
    float countdownTimer = levelManager->GetCountdown(); //this could be used to display a countdown on screen, for example.
    if (countdownOver) {
        soundManager->SM_PlaySound(soundManager->GetCurrentSong());
        state = GameplayStateEnums::PLAYING;
        canvas->PopActiveLayer();
        soundManager->SM_PlaySound("sfx_go.wav");
        ResetCameraToForwards();
    }
}

void GameplayState::UpdateAndRenderWorld(float dt) {
    ReadNetworkFunctions();

    Window::GetWindow()->ShowOSPointer(isPaused);
    if (!isPaused)
    {
        UpdateGrapples(dt);
        
        if (firstPersonPosition && state != GameplayStateEnums::COUNTDOWN) {
            world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
        }
        WalkCamera(dt);
        if (jumpTimer > 0) JumpCamera(dt);
        if (landTimer > 0) LandCamera(dt);
        StrafeCamera(dt);
        if (state == GameplayStateEnums::COUNTDOWN)
        {
            cinematicCamera->UpdateCinematicCamera(world->GetMainCamera());
        }
        else
        {
            world->GetMainCamera()->UpdateCamera(dt);
        }

        totalDTElapsed += dt;
        UpdateGrapples(dt);

        Window::GetWindow()->ShowOSPointer(false);

        if (firstPersonPosition && state != GameplayStateEnums::COUNTDOWN) {
            world->GetMainCamera()->SetPosition(firstPersonPosition->GetPosition());
        }
        world->UpdateWorld(dt);
    }
    if(!baseClient->IsSinglePlayer())    UpdatePositionRankings();
    ResetCameraAnimation();
    ReadNetworkPackets();

    // particle updates
    UpdateParticleSystems(dt);

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) displayDebugger = !displayDebugger;
    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I)) cinematicCamera->WriteCameraInfo(world->GetMainCamera(), "autocamera.txt");
    if (displayDebugger) DebugMode::UpdateDebugMode(dt);
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
}

void GameplayState::UpdatePlaying(float dt){
    SendInputData();
    UpdateAndRenderWorld(dt);
}

void GameplayState::UpdatePlayerCompleted(float dt)
{
    UpdateAndRenderWorld(dt);
}

void GameplayState::UpdateEndOfLevel(float dt)
{
    if (shouldMoveToNewLevel) {
        OnNewLevel();
        canvas->PushActiveLayer("StartScreenLayer");
        shouldMoveToNewLevel = false;
        Window::GetWindow()->ShowOSPointer(false);
        state = GameplayStateEnums::COUNTDOWN;
        cinematicCamera->ResetCurrentCamera();
        cinematicCamera->ResetTimer();
    }
    UpdateAndRenderWorld(dt);
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
                selfID = handler.Unpack<int>();
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
                
                soundManager->SM_PlaySound("sfx_walk.wav");
                soundManager->SM_SetSoundVolume("sfx_walk.wav", 0.5f);
                float randomPitchMod = (float)(rand()) / (float)(RAND_MAX);
                soundManager->SM_SetSoundPitch("sfx_walk.wav", 1.0f - randomPitchMod * 0.25f);

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
                OnEndReached(handler);

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
                if (shouldMoveToNewLevel)continue; //dont do this if we need to load a new level

                int eventType = handler.Unpack<int>();
                HandleGrappleEvent(eventType);
            } break;

            case(Replicated::Player_Velocity_Call): {
                if (shouldMoveToNewLevel)continue; //dont do this if we need to load a new level

                Vector3 velocity = handler.Unpack<Vector3>();
                playerVelocity = velocity;
                float speed = std::max(0.0f, velocity.Length() - 10.0f);
                float speedVisualModifier = std::min(speed, 50.0f) / 50.0f;
                renderer->SetSpeedLineAmount(speedVisualModifier);
                world->GetMainCamera()->SetFieldOfVision( defaultFOV + speedVisualModifier * 30.0f);
            }
            break;

            case(Replicated::GameInfo_Timer): {
                float recievedTime = handler.Unpack<float>();
                timeElapsed = recievedTime;

                for (int i = 0; i < 3; i++) {
                    recievedTime = handler.Unpack<float>();
                    medalTimes[i] = recievedTime;
                }
                timerRatio = 1 - std::clamp(timeElapsed, 0.0f, medalTimes[2]) / medalTimes[2];
                
                int recievedMedal = handler.Unpack<int>();
                if (currentMedal != recievedMedal) {
                    timerMedalShakeTimer = 1.0f;
                    currentMedal = recievedMedal;
                    Vector4 colours[4] = { Replicated::PLATINUM, Replicated::GOLD, Replicated::SILVER, Replicated::BRONZE };
                    timerBarColor = colours[ currentMedal ];
                }
            }
            break; 

            case(Replicated::SendMedalValues): {
                auto medals = handler.Unpack<Vector3>();
                for (int i = 0; i < 3; i++)medalTimes[i] = medals[i];
                timeElapsed = 0;
            }
            break;
            
            case(Replicated::SetNetworkActive): {
                int networkObjectId = handler.Unpack<int>();
                bool isActive = handler.Unpack<bool>();
                auto targetObject = world->GetObjectByNetworkId(networkObjectId);
                targetObject->SetActive(isActive);
            } break;

            case(Replicated::Player_Animation_Call): {
                if (shouldMoveToNewLevel)continue; //dont do this if we need to load a new level

                Replicated::RemoteAnimationData data = handler.Unpack< Replicated::RemoteAnimationData>();
                UpdatePlayerAnimation(data.networkID, data.state);
            }
            break;

            case(Replicated::Load_Level): {
                int level = handler.Unpack<int>();
                numberPlayersJoined = handler.Unpack<int>();
                levelManager->SetHasReceivedLevel(true);
                levelManager->ChangeLevel(level);
                shouldMoveToNewLevel = true;
            }
            break;

            case(Replicated::All_Players_Finished): {
                state = GameplayStateEnums::END_OF_LEVEL;
                soundManager->SM_StopSound(soundManager->GetCurrentSong()); //perhaps play some different music or sound effect

                //show a scoreboard?
                }
                break;

            case(Replicated::GameInfo_GrappleAvailable): {
                int eventType = handler.Unpack<int>();
                if (eventType == 1) {
                    crossHairRotation = 45.0f * rotationDirection;
                    currentCHRotation = crossHairRotation;
                    crossHairScale = 1.25f;
                    soundManager->SM_PlaySound("sfx_chainready.wav");
                }
                else {
                    crossHairRotation = 0.0f;
                    crossHairScale = 1.15f;
                    soundManager->SM_PlaySound("sfx_chainthrow.wav");
                }
            } 
            break;

            case(Replicated::GameInfo_PlayerPositions): {
                int unpackFlag = 0;
                
                while (unpackFlag != -999) {
                    unpackFlag = handler.Unpack<int>();
                    if (unpackFlag == -999) continue;

                    std::string unpackFlagID = "blip_" + std::to_string(unpackFlag);
                    if (playerPositions.find(unpackFlagID) == playerPositions.end()) {
                        playerPositions.insert({ unpackFlagID, Vector3()});
                        InitPlayerBlip(unpackFlag);
                    }
                    Vector3 position = handler.Unpack<Vector3>();
                    playerPositions[unpackFlagID] = position;
                    
                }
                
            }
        }
    }
}


void GameplayState::UpdatePositionRankings() {
    if (playerPositions.size() == 0) return;
    int rankPlacement = 0;
    float selfPos = CalculateCompletion(playerPositions["blip_" + std::to_string(selfID)]);
    for (int i = 0; i < playerPositions.size(); i++) {
        if (i == selfID) continue;
        if (selfPos < CalculateCompletion(playerPositions["blip_" + std::to_string(i)])) {
            rankPlacement++;
        }
    }
    switch (rankPlacement + 1) {
    case(Medal::Gold):
        positionColor = Replicated::GOLD;
        break;

    case(Medal::Silver):
        positionColor = Replicated::SILVER;
        break;

    case(Medal::Bronze):
        positionColor = Replicated::BRONZE;
        break;

    default:
        positionColor = Replicated::DEFAULT;
        break;
    }
}
void GameplayState::OnEndReached(DataHandler& handler)
{
    int networkId = handler.Unpack<int>();
    int medal = handler.Unpack<int>();
    Vector4 medalColour = handler.Unpack<Maths::Vector4>();
    medalColour.w = 0.0f;
    if (!hasReachedEnd) InitEndScreen(medalColour);
    if (state != GameplayStateEnums::END_OF_LEVEL)state = GameplayStateEnums::PLAYER_COMPLETED; //just in case the packets arrive out of order
    float speedVisualModifier = 0;
    renderer->SetSpeedLineAmount(speedVisualModifier);
    world->GetMainCamera()->SetFieldOfVision(defaultFOV + speedVisualModifier * 30.0f);

    soundManager->SM_StopSound(soundManager->GetCurrentSong()); //perhaps play some different music or sound effect
}

void GameplayState::UpdatePlayerAnimation(int networkID, Replicated::PlayerAnimationStates state) {
    GameObject *playerObject = world->GetObjectByNetworkId(networkID);
    if (!playerObject)return;

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
    groundedMovementSpeed = groundedMovementSpeed * 0.95f;
    strafeSpeed = 0.0f;
}

GameObject* GameplayState::CreateChainLink() {
    auto g = new GameObject("chain");
    world->AddGameObject(g, false);
    g->GetTransform().SetPosition({0,0,0}).SetScale({chainSize, chainSize, chainSize});
    g->SetRenderObject(new RenderObject(g->GetTransformPointer(), resources->GetMesh("chainLink.obj"), resources->GetTexture("FlatColors.png"), nullptr));
    g->GetRenderObject()->SetShouldInstance(true);
    g->SetActive(false);
    return g;
}

void GameplayState::CreateChains() {
    for (int i=0; i < chainLinkCount * numberPlayersJoined; i++) {
        chains[i] = CreateChainLink();
        //chains[i]->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 90 * (i % 2)));
    }
}

void GameplayState::OperateOnChains(int grappleIndex, const std::function<void(GameObject&, int)>& opFunction) {
    for (int i = 0; i < chainLinkCount; i++) {
        int chainLinkIndex = grappleIndex * chainLinkCount + i;
        opFunction(*chains[chainLinkIndex], i);
    }
}

void GameplayState::WalkCamera(float dt) {
    world->GetMainCamera()->SetOffsetPosition(Vector3(0, abs(bobFloor + bobAmount *sin(walkTimer)) * (groundedMovementSpeed / maxMoveSpeed), 0));
    
    groundedMovementSpeed = groundedMovementSpeed * 0.95 + currentGroundSpeed * 0.05;
    if (walkSoundTimer <= 0) {
        
        std::string walkSound = walkSounds[rand() % walkSounds.size()];
        soundManager->SM_PlaySound(walkSound);
        soundManager->SM_SetSoundVolume(walkSound, 0.25f);
        float randomPitchMod = ((-0.5f + (float)(rand()) / (float)(RAND_MAX))) * 0.5f;
        soundManager->SM_SetSoundPitch(walkSound, 1.0f + randomPitchMod);
        walkSoundTimer += PI;
    }
    float bobHeight = abs(bobFloor + bobAmount * sin(walkTimer));
    world->GetMainCamera()->SetOffsetPosition(Vector3(0, bobHeight * (groundedMovementSpeed / maxMoveSpeed), 0));
    walkTimer += dt * groundedMovementSpeed * 0.75f;
    walkSoundTimer -= dt * groundedMovementSpeed * walkSoundTimerMultiplier;
}

void GameplayState::JumpCamera(float dt) {
    world->GetMainCamera()->SetOffsetPosition(world->GetMainCamera()->GetOffsetPosition() + Vector3(0, -jumpBobAmount * sin(PI - jumpTimer), 0));
    jumpTimer = std::clamp(jumpTimer - dt * jumpAnimationSpeed, 0.0f, PI);
}

void GameplayState::HandleGrappleEvent(int event) {
    switch (event) {
        case 1: {
            isGrappling = true;
            soundManager->SM_PlaySound("sfx_chainhook.wav");
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

    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
        if (isSinglePlayer && !hasReachedEnd)
        {
            TogglePause();
        }
    }

    if (isPaused) return;
    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)){
        networkData->outgoingFunctions.Push(FunctionPacket(Replicated::PlayerJump, nullptr));
    }

    if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT)) {
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
    rotationDirection = input.playerDirection.x == 0 ? rotationDirection : (input.playerDirection.x > 0 ? -1 : 1);
    networkData->outgoingInput.Push(input);
}

void GameplayState::TogglePause() {
    networkData->outgoingFunctions.Push(FunctionPacket(Replicated::RemoteServerCalls::Pause, nullptr));
    isPaused = !isPaused;
    Window::GetWindow()->LockMouseToWindow(!isPaused);
    renderer->SetSpeedActive(!isPaused);
    if (isPaused) {
        canvas->PushActiveLayer("PauseLayer");
        soundManager->SM_PauseSound(soundManager->GetCurrentSong());
    }
    else {
        canvas->PopActiveLayer();
        soundManager->SM_ResumeSound(soundManager->GetCurrentSong());
    }
}

void GameplayState::FinishLoading() {
    while (worldHasLoaded != LoadingStates::LOADED && finishedLoading != LoadingStates::READY) {
    }
    shouldLoadScreen.store(false);
    InitCanvas();
    world->StartWorld();
    networkData->outgoingFunctions.Push(FunctionPacket(Replicated::GameLoaded, nullptr));
}

void GameplayState::InitCamera() {
    Camera* cam = world->GetMainCamera();
    cam->SetNearPlane(0.1f);
    cam->SetFarPlane(500.0f);
    cam->SetPitch(-15.0f);
    cam->SetYaw(315.0f);
    cam->SetPosition(Vector3(-60, 40, 60));
    cam->SetCameraOffset(Vector3(0, 0.5f,0 )); //to get the camera to the player's head

    cinematicCamera = new CinematicCamera();
    cinematicCamera->ReadPositionsFromFile("autocamera.txt");
}

void GameplayState::InitWorld() {
    InitLevel(levelManager->GetLevel());
    CreatePlayers();
    CreateGrapples();
    CreateChains();
    renderer->SetPointLights(world->GetPointLights());

    worldHasLoaded = LoadingStates::LOADED;
}

void GameplayState::InitEndScreen(Vector4 color) { // should be renamed
    hasReachedEnd = true;
    ResetEndScreenAnimTimers();
    finalTime = timeElapsed;
    medalAnimationStage = MedalAnimationStages::TIMER_SCROLL;
    canvas->PushActiveLayer("FinishedLevelLayer");
    canvas->GetElementByIndex(endMedalElementIndex, "FinishedLevelLayer").SetColor(color);
    // Disable player controls
    // Clear the world
    // Loading screen
    // Load the next level
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

int CGetDirectionFromPlayerNumber(int num) {
    return (((num % 2) * 2) - 1); // 0 = -1, 1 = 1
}

int CGetMagnitudeFromPlayerNumber(int num) {
    return num < 3 ? 1 : 3; // Uses player number to adjust how far from other players.
}

void GameplayState::LoadPlayerMeshMaterials() {
    playerTextures[0] = resources->GetMeshMaterial("Player1.mat");
    playerTextures[1] = resources->GetMeshMaterial("Player2.mat");
    playerTextures[2] = resources->GetMeshMaterial("Player3.mat");
    playerTextures[3] = resources->GetMeshMaterial("Player4.mat");
}

void GameplayState::CreatePlayers() {
    float playerSeperation = 2.0f;
    int currentPlayer = 0;

    OGLShader* playerShader = new OGLShader("SkinningVert.vert", "Player.frag");
    MeshGeometry* playerMesh = resources->GetMesh("Player.msh");
    Vector3 startPos = levelManager->GetLevelReader()->GetStartPosition();
    for (int i=0; i<numberPlayersJoined; i++) {
        Vector3 thisPlayerStartPos = startPos + Vector3(0, 0, 1) * CGetDirectionFromPlayerNumber(currentPlayer) * CGetMagnitudeFromPlayerNumber(currentPlayer) * playerSeperation;
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);
  
        player->GetTransform().SetPosition(thisPlayerStartPos);
        playerMesh->AddAnimationToMesh("Run", resources->GetAnimation("Player_FastRun.anm"));
        playerMesh->AddAnimationToMesh("LeftStrafe", resources->GetAnimation("Player_RightStrafe.anm")); //this is just how the animations were exported
        playerMesh->AddAnimationToMesh("RightStrafe", resources->GetAnimation("Player_LeftStrafe.anm"));
        playerMesh->AddAnimationToMesh("RunBack", resources->GetAnimation("Player_RunBack.anm"));
        playerMesh->AddAnimationToMesh("Idle", resources->GetAnimation("Player_Idle.anm"));
        playerMesh->AddAnimationToMesh("Fall", resources->GetAnimation("Player_Fall.anm"));
        playerMesh->AddAnimationToMesh("Jump", resources->GetAnimation("Player_Grapple.anm"));
        player->SetRenderObject(new RenderObject(&player->GetTransform(), playerMesh, nullptr, playerShader));
        player->GetRenderObject()->SetMeshScale(player->GetTransform().GetScale() * 1.0f);
        player->GetRenderObject()->SetMeshOffset(Vector3(0,-0.5f,0));

        AnimatorObject* newAnimator = new AnimatorObject(playerMesh->GetAnimationMap());
        newAnimator->SetAnimation(playerMesh->GetAnimation("Idle"));
        newAnimator->SetMidPose("Idle");
        player->SetAnimatorObject(newAnimator);
        player->GetRenderObject()->SetAnimatorObject(newAnimator);
        player->GetRenderObject()->SetMeshMaterial(playerTextures[currentPlayer]);
        std::cout << player->GetNetworkObject()->GetNetworkId() << std::endl;
        currentPlayer++;
    }
}

void GameplayState::InitLevel(int level) {
    auto plist  = levelManager->GetLevelReader()->GetPrimitiveList();
    auto opList  = levelManager->GetLevelReader()->GetOscillatorPList();
    auto harmOpList  = levelManager->GetLevelReader()->GetHarmfulOscillatorPList();
    auto swingpList = levelManager->GetLevelReader()->GetSwingingPList();
    auto springList  = levelManager->GetLevelReader()->GetSpringPList();
    auto lightList  = levelManager->GetLevelReader()->GetPointLights();

    auto speedUpList = levelManager->GetLevelReader()->GetSpeedupBlockPrimitiveList();
    auto bridgeList = levelManager->GetLevelReader()->GetBridgePrimitiveList();
    auto trapBlockList = levelManager->GetLevelReader()->GetTrapBlockPrimitiveList();
    auto rayEnemyList = levelManager->GetLevelReader()->GetRayEnemyPrimitiveList();
    auto rayenemyTriList = levelManager->GetLevelReader()->GetRayTriggerPrimitiveList();
    auto bridgeTriList = levelManager->GetLevelReader()->GetBridgeTriggerPrimitiveList();

    for(auto &x : plist){
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
//        temp->GetRenderObject()->SetColour({0.0f, 0.65f, 0.90f, 1.0f});

    }

    for (auto &x : opList) {
        auto temp = new GameObject("Oscillator");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.5f,0.0f, 1.0f });
    }

    for (auto &x : harmOpList) {
        auto temp = new GameObject("Harmful");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.0f,0.0f, 1.0f });
    }
    for (auto& x : springList) {
        auto temp = new GameObject("Spring");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 0.0f, 1.0f,0.0f, 1.0f });
    }

    for (auto& x : swingpList)
    {
        auto temp = new GameObject("Swinging");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 0.0f, 1.0f,0.0f, 1.0f });
    }
    
    for (auto& l : lightList) {
        AddPointLight(l);
    }

    for (auto& x : speedUpList) {
        auto temp = new GameObject("SpeedUp");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 0.0f, 1.0f,0.0f, 1.0f });
    }

    for (auto& x : bridgeList) {
        auto temp = new GameObject("Bridge");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 0.0f, 1.0f,1.0f, 1.0f });
    }

    for (auto& x : trapBlockList) {
        auto temp = new GameObject("TrapBlock");
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.5f, 0.0f, 1.0f });
    }

    OGLShader* playerShader = new OGLShader("SkinningVert.vert", "Player.frag");  //this needs to be reworked as this will not be freed

    for (auto& x : rayEnemyList) {
        auto temp = new GameObject("RayEnemy");
        MeshGeometry* maxMesh = resources->GetMesh("Rig_Maximilian.msh");
        maxMesh->AddAnimationToMesh("Default", resources->GetAnimation("Max_Idle.anm"));
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), maxMesh, nullptr, playerShader));
  
        temp->GetRenderObject()->SetMeshMaterial(resources->GetMeshMaterial("Rig_Maximilian.mat"));
        temp->GetRenderObject()->SetColour({ 1.0f, 1.0f, 1.0f, 1.0f });
        temp->GetRenderObject()->SetMeshOffset(Vector3(0,-1.7f,0));

        temp->SetAnimatorObject(new AnimatorObject(maxMesh->GetAnimationMap()));
        temp->GetAnimatorObject()->SetAnimation("Default");

        temp->GetRenderObject()->SetAnimatorObject(temp->GetAnimatorObject());

    }

    for (auto& x : rayenemyTriList) {
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 1.0f, 0.0f, 0.0f, 1.0f });
    }

    for (auto& x : bridgeTriList) {
        auto temp = new GameObject();
        replicated->AddBlockToLevel(temp, *world, x);
        temp->SetRenderObject(new RenderObject(&temp->GetTransform(), resources->GetMesh(x->meshName), nullptr, nullptr));
        temp->GetRenderObject()->SetColour({ 0.0f, 1.0f, 0.0f, 1.0f });
    }


    levelLen = (levelManager->GetLevelReader()->GetEndPosition() - levelManager->GetLevelReader()->GetStartPosition()).Length();
    startPos = levelManager->GetLevelReader()->GetStartPosition();
    endPos = levelManager->GetLevelReader()->GetEndPosition();

    deathPos = levelManager->GetLevelReader()->GetDeathBoxPosition();
    endPos = levelManager->GetLevelReader()->GetEndPosition();
    
    RenderFlairObjects();
}

void GameplayState::OnGrappleToggle(GameObject& gameObject, bool isActive) {
    int id = gameObject.GetNetworkObject()->GetNetworkId() % numberPlayersJoined;
    if (!isActive) OperateOnChains(id, [](GameObject& chainLink, int chainIndex) {
        chainLink.SetActive(false);
    });

    if (isActive) OperateOnChains(id, [](GameObject& chainLink, int chainIndex) {
        chainLink.SetActive(true);
    });
}

void GameplayState::UpdateGrapples(float dt) {
    if (isGrappling) {
        grappleContVolume = std::clamp(grappleContVolume + dt * 1.5f, 0.0f, 1.0f);
    }
    else {
        grappleContVolume = std::clamp(grappleContVolume - dt * 1.5f, 0.0f, 1.0f);
    }
    if (!soundManager->SM_IsSoundPlaying("sfx_chainreel.wav") && grappleContVolume > 0.0f) {
        soundManager->SM_PlaySound("sfx_chainreel.wav");
        soundManager->SM_SetSoundVolume("sfx_chainreel.wav", grappleContVolume * 0.25f);
    }
    if(soundManager->SM_IsSoundPlaying("sfx_chainreel.wav")) {
        soundManager->SM_SetSoundVolume("sfx_chainreel.wav", grappleContVolume * 0.25f);
    }

    for (GameObject* grapple: grapples) {


        int id = grapple->GetNetworkObject()->GetNetworkId() % numberPlayersJoined;
        auto playerObject = world->GetNetworkObject(grapple->GetNetworkObject()->GetNetworkId() - numberPlayersJoined)->GetParent();
        if (!chains[id * chainLinkCount]->IsActive()) continue;

        const Vector3 &playerPos = playerObject->GetTransform().GetPosition() + Matrix3(playerObject->GetTransform().GetOrientation()) * Replicated::HANDOFFSET;
        const Vector3 &grapplePos = grapple->GetTransform().GetPosition();

        Vector3 chainVector = (grapplePos - playerPos).Normalised() * Replicated::GRAPPLEDISTANCE / chainSize / 4;
        auto rotation = Matrix3::LookAt(playerPos, grapplePos, Vector3(0, 1, 0));

        OperateOnChains(id, [=](GameObject& chainLink, int chainIndex) {
            float chainTValue = (float)chainIndex / (float)chainLinkCount;
            Vector3 linkVector = chainVector * chainTValue;
            Vector3 adjustment = linkVector.LengthSquared() < (grapplePos - playerPos).LengthSquared() ? linkVector : grapplePos - playerPos;
            chainLink.GetTransform()
                .SetPosition(playerPos + adjustment)
                .SetOrientation(Quaternion(rotation).Normalised() * Quaternion::EulerAnglesToQuaternion(0, 0, 90 * (chainIndex % 2)));
        });
    }
}

void GameplayState::CreateGrapples() {
    for (int i = 0; i < numberPlayersJoined; i++) {
        auto g = new GameObject();
        replicated->AddGrapplesToWorld(g, *world, i);
        g->SetRenderObject(new RenderObject(&g->GetTransform(), resources->GetMesh("trident.obj"), resources->GetTexture("FlatColors.png"), nullptr));
        g->OnActiveSet.connect<&GameplayState::OnGrappleToggle>(this);
        grapples.push_back(g);
    }

}

void GameplayState::SetTestSprings() {
    for (int i = 0; i < 4; i++) {
        auto g = new GameObject();
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -40.0f));
        g->SetRenderObject(new RenderObject(&g->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));
        g->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f / 4.0f * i, 0.0f, 1.0f));

        PointLightInfo light;
        light.lightColour = Vector4(1.0f, 1.0f / 4.0f * i, 0.0f, 1.0f);
        light.lightPosition = Vector3(-40.0f + 15.0f * i, 0.0f, -40.0f);
        light.lightRadius = 7.0f;
        world->AddPointLightToWorld(light);
    }

    for (int i = 0; i < 4; i++) {
        auto g = new GameObject();
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -50.0f));
        g->SetRenderObject(new RenderObject(&g->GetTransform(), resources->GetMesh("Cube.msh"), nullptr, nullptr));
        g->GetRenderObject()->SetColour(Vector4(0, 1.0f / 4.0f * i, 1.0f, 1.0f));

        PointLightInfo light;
        light.lightColour = Vector4(0, 1.0f / 4.0f * i, 1.0f, 1.0f);
        light.lightPosition = Vector3(-40.0f + 15.0f * i, 0.0f, -50.0f);
        light.lightRadius = 7.0f;
        world->AddPointLightToWorld(light);
    }

}

void GameplayState::AddPointLight(PointLightInfo light) {
    world->AddPointLightToWorld(light);
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

    //cinematicCamera->AddInitialCamera(levelManager->GetLevelReader()->GetStartPosition());
    cinematicCamera->AddInitialCamera(firstPersonPosition->GetPosition() + world->GetMainCamera()->GetOffsetPosition());
}

float GameplayState::CalculateCompletion(Vector3 playerCurPos){

    auto progress = playerCurPos - endPos;
    return 1 - progress.Length()/levelLen;
}

void GameplayState::UpdateStartBack(Element& element, float dt) {
    if (levelManager->GetCountdown() > whenToStartCountdown) {
        element.SetColor({ 0,0,0,0 });
        return;
    }
    float backAlpha = element.GetColor().w;
    float targetBackAlpha = 0.75;
    backAlpha = backAlpha * 0.9f + targetBackAlpha * 0.1f;
    element.SetColor({ 0,0,0,backAlpha });

}

void GameplayState::UpdateStartText(Element& element, float dt) {
    float endTimer = levelManager->GetCountdown();
    if (endTimer > whenToStartCountdown) {
        element.textData.text = "";
        return;
    }
    std::string id = element.GetId();
    if (id == "StartScreen_Countdown") {
        if (countdownCurrentInt != (int)ceil(endTimer)) {
            countdownCurrentInt = (int)ceil(endTimer);
            soundManager->SM_PlaySound("sfx_countdown.wav");
            soundManager->SM_SetSoundPitch("sfx_countdown.wav", 1.0f + (3 - countdownCurrentInt) * 0.3f);
        }
        element.textData.text = std::to_string((int)ceil(endTimer));
    }
    else if (id == "StartScreen_LevelName") {
        std::string levelName = levelManager->GetLevelName(levelManager->GetLevel());
        int characters = levelName.length();
        element.SetAbsoluteSize({ 40 * characters, 50 });
        element.AlignMiddle(150);
        element.AlignCenter();
        element.textData.text = levelName;
    }
}

void GameplayState::UpdateCrosshair(Element& element, float dt) {
    element.GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, currentCHRotation));
    element.SetTransformTranslation(Vector2(50, 50));
    element.GetTransform().SetScale(Vector3(crossHairScale, crossHairScale, 1));
    element.SetColor(crossHairRotation == 0 ? Vector4(0, 0, 1, 1) : Vector4(1, 1, 1, 1));
    crossHairScale = std::clamp(crossHairScale - dt, 1.0f, 100.0f);
    currentCHRotation = currentCHRotation * 0.9f + crossHairRotation * 0.1f;
}

void GameplayState::UpdateTimerUI(Element& element, float dt) {
    if (medalTimes[0] == -1.0f) return;

    element.SetAbsoluteSize({ (int)round((800 - 20) * timerRatio), timerBarHeight });
    element.SetColor(timerBarColor);
}

void GameplayState::UpdateTimerBox(Element& element, float dt) {
    if (isSinglePlayer) {
        if (medalTimes[0] == -1.0f) return;
        float positionRatio = timerRatio;
        element.AlignCenter((int)round(-400 + 30 / 2 + (800 - 30) * positionRatio))
            .SetColor(timerBarColor);
        return;
    }

    element.SetColor(positionColor);

}

void GameplayState::UpdateTimerText(Element& element, float dt) {
    if (medalTimes[0] == -1.0f) return;
    element.textData.text = std::format("{:.2f}", timeElapsed);
    element.textData.fontSize = 0.5f;

    int randomX = (int)round((-0.5f + (float)(rand()) / (float)(RAND_MAX)) * timerMedalShakeTimer * 4);
    int randomY = (int)round((-0.5f + (float)(rand()) / (float)(RAND_MAX)) * timerMedalShakeTimer * 4);
    if (!isSinglePlayer) {
        randomX = 0;
        randomY = 0;
    }
    element.AlignCenter(400 + randomX);
    element.AlignTop(timerTopOffset + timerBarHeight / 2 + 8 + randomY);
    
    timerMedalShakeTimer = std::clamp(timerMedalShakeTimer - dt, 0.0f, 1.0f);
}

void GameplayState::UpdateTimerNub(Element& element, float dt) {
    if (medalTimes[0] == -1.0f) return;
    int medalID = medalTimeRatios[element.GetId()].first;
    if (medalTimeRatios[element.GetId()].second < 0.0f) {
        medalTimeRatios[element.GetId()].second = 1 - std::clamp(medalTimes[medalID], 0.0f, medalTimes[2]) / medalTimes[2];
        element.AlignCenter((int)round(-400 + (800 - timerBoxWidth) * medalTimeRatios[element.GetId()].second + timerBarOutline - 1));
    }

}

void GameplayState::UpdateMedalSprite(Element& element, float dt) {
    
    if (medalAnimationStage != MedalAnimationStages::MEDAL) return;
    Vector4 currentColor = element.GetColor();
    float spinAmount = 360.0f * 2.0f;
    float timeToTake =  0.2f;
    float scaleFactor = 1.0f / (spinAmount)*medalTimer;
    element.SetColor(Vector4(currentColor.x, currentColor.y, currentColor.z, scaleFactor));
    medalTimer = std::clamp(medalTimer + dt * (spinAmount) / timeToTake, 0.0f, spinAmount);

    element.GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, medalTimer));
    element.GetTransform().SetScale(Vector3(2 - scaleFactor, 2 - scaleFactor,1));
    if (medalTimer == spinAmount) {
        element.SetColor(Vector4(currentColor.x, currentColor.y, currentColor.z, 1));
        medalAnimationStage = MedalAnimationStages::FINISHED;
    }
}

void GameplayState::UpdateFinalTimeTally(Element& element, float dt) {
    float textSize = 2.0f;
    switch (medalAnimationStage)
    {
    case(MedalAnimationStages::TIMER_SCROLL): {
        
        float timeToScroll = finalTime / 1.5f;
        float timeRatio = (finalTimeScroll / finalTime);
        finalTimeScroll = std::clamp(finalTimeScroll + dt * timeToScroll, 0.0f, finalTime);
        element.textData.text = std::format("{:.2f}", finalTimeScroll);
        element.textData.fontSize = textSize * 0.33f;

        element.AlignCenter();
        element.AlignMiddle(-24 * textSize * 0.33f);
        element.textData.color = Vector4(1.0f, 1.0f, 1.0f, 0.10f);
        finalTimeSoundRepeat -= dt;
        if (finalTimeSoundRepeat <= 0) {

            soundManager->SM_PlaySound("sfx_timetally.wav");
            float pitchIncrease = 1.0f + 0.5 * (finalTimeScroll / finalTime);
            soundManager->SM_SetSoundPitch("sfx_timetally.wav", pitchIncrease);
            finalTimeSoundRepeat = 0.07f;
        }
        if (finalTimeScroll == finalTime) {
            soundManager->SM_PlaySound("sfx_timeshake.wav");
            medalAnimationStage = TIMER_SHAKE;
            finalTimeShake = 1.0f;
        }
    }
    break;
    case(MedalAnimationStages::TIMER_SHAKE): {
        int randomX = (int)round((-0.5f + (float)(rand()) / (float)(RAND_MAX)) * finalTimeShake * 4);
        int randomY = (int)round((-0.5f + (float)(rand()) / (float)(RAND_MAX)) * finalTimeShake * 4);
        finalTimeShake = std::clamp(finalTimeShake - dt, 0.0f, 1.0f);
        finaltimeShrink = std::clamp(finaltimeShrink - dt * 4.5f, 0.0f, 1.0f);
        textSize = 2.5f - 0.5f * cos( PI/2 * (finaltimeShrink));
        element.textData.fontSize = textSize * 0.33f;
        element.AlignCenter(randomX);
        element.AlignMiddle(-24 * textSize * 0.33f + randomY);
        if (finalTimeShake == 0.0f) {
            medalAnimationStage = MEDAL;
            soundManager->SM_PlaySound("sfx_medal.wav");
        }
    }
    break;
    }
    
}

void GameplayState::OnPauseHoverEnter(Element &element) {

    element.textData.color = { 1,1,1,1 };
    std::string elementID = element.GetId();
    if (elementID == "Resume") {
        selectedPause = 0;
    }
    if (elementID == "Exit") {
        selectedPause = 1;
    }
}

void GameplayState::OnPauseHoverExit(Element& element) {

    element.textData.color = { 0.5,0.5,0.5,1 };
}

void GameplayState::OnPauseClick(Element& element) {
    std::string elementID = element.GetId();
    if (elementID == "Resume") {
        TogglePause();
    }
    
}

void GameplayState::UpdatePauseFlame(Element& element, float dt) {
    float direction = -1.0f;
    if (element.GetId() == "pauseflame_1") direction = 1.0f;
    float setToX = 0;
    float setToY = 0;
    switch (selectedPause)
    {
    case(0):
        setToX = 300.0f;
        setToY = 75.0f;
        break;

    case(1):
        setToX = 225.0f;
        setToY = -75.0f;
        break;
    }
    flameToXGap = flameToXGap * 0.9f + setToX * 0.1f;
    flameToY = flameToY * 0.9f + setToY * 0.1f;
    element.AlignCenter((int)round(flameToXGap * direction));
    element.AlignMiddle((int)round(flameToY) + 25);
}

void GameplayState::RenderFlairObjects(){
    Vector3 lavaPos = deathPos ;

    AddLava(lavaPos + Vector3(0, 5, 0));
    AddEndPortal(endPos);

}

void GameplayState::AddLava(Vector3 position){
    auto LavaQuad = new GameObject("LavaQuad");
    LavaQuad->GetTransform().SetOrientation(Quaternion(Matrix4::Rotation(90, {1,0,0})));
    replicated->AddTestObjectToLevel(LavaQuad, *world, {1000,1000,1000}, position, false);
    LavaQuad->SetRenderObject(new RenderObject(&LavaQuad->GetTransform(), resources->GetMesh("Quad.msh"), resources->GetTexture("VorDef.png"), resources->GetShader("lava")));
    renderer->SetLavaHeight(position.y);
    //LavaQuad->GetRenderObject()->SetDepthTest(false);
}

void GameplayState::AddEndPortal(Vector3 position){

    auto PortalQwaud = new GameObject("Portal");
    PortalQwaud->GetTransform().SetOrientation(Quaternion(Matrix4::Rotation(90, { 0,1,0 })));
    replicated->AddTestObjectToLevel(PortalQwaud, *world, { 10,10,10 }, endPos + Vector3(0.0f,2.5f,0.0f), false);
    PortalQwaud->SetRenderObject(new RenderObject(&PortalQwaud->GetTransform(), resources->GetMesh("Quad.msh"), resources->GetTexture("VorDef.png"), resources->GetShader("portal")));
    PortalQwaud->GetRenderObject()->SetDepthTest(false);
}
