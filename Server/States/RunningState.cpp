#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;

#define GRAPPLE_SWAY_MULTIPLIER 15.0f

RunningState::RunningState(GameServer* pBaseServer) : State() {
    // Don't use serverBase without talking to other members of the team.
    serverBase = pBaseServer;
    replicated = std::make_unique<Replicated>();
    world = std::make_unique<GameWorld>();
    physics = std::make_unique<PhysicsSystem>(*world);
    levelManager = std::make_unique<LevelManager>();
    shouldClose.store(false);

    currentLevelDeathPos = {0,0,0};
}

RunningState::~RunningState() {
    shouldClose.store(true);
    networkThread->join();
}

void RunningState::OnEnter() {
    //serverBase has been called before the network thread has been created.
    serverBase->CallRemoteAll(Replicated::RemoteClientCalls::LoadGame, nullptr);
    playerInfo = serverBase->GetPlayerInfo();
    sceneSnapshotId = 0;
    numPlayersLoaded = 0;

    shouldClose.store(false);

    CreateNetworkThread();
    LoadLevel();
    world->StartWorld();

}

void RunningState::ThreadUpdate(GameServer* server, ServerNetworkData* networkData) {
    auto threadServer = ServerThread(server, networkData);

    while (!shouldClose) {
        threadServer.Update();
    }
}

void RunningState::CreateNetworkThread() {
    GameServer* server = serverBase;
    networkData = std::make_unique<ServerNetworkData>();
    networkThread = new std::thread(&RunningState::ThreadUpdate, this, server, networkData.get());
}

void RunningState::ReadNetworkFunctions() {
    while (!networkData->incomingFunctions.IsEmpty()) {
        auto data = networkData->incomingFunctions.Pop();
        if (data.second.functionId == Replicated::RemoteServerCalls::GameLoaded) {
            AssignPlayer(data.first, GetPlayerObjectFromId(data.first));
            numPlayersLoaded++;
        }
        else if(data.second.functionId == Replicated::RemoteServerCalls::PlayerJump){
            auto player = GetPlayerObjectFromId(data.first);
            PlayerMovement* playerMovement;
            if (player->TryGetComponent(playerMovement)) {
                playerMovement->Jump();
            }
        }
        else if (data.second.functionId == Replicated::RemoteServerCalls::PlayerGrapple) {
            auto player = GetPlayerObjectFromId(data.first);
            PlayerMovement* playerMovement;
            if (player->TryGetComponent(playerMovement)) {
                playerMovement->Grapple();
            }
        }
        else if (data.second.functionId == Replicated::RemoteServerCalls::PlayerDebug) {
            auto player = GetPlayerObjectFromId(data.first);
            PlayerMovement* playerMovement;
            if (player->TryGetComponent(playerMovement)) {
                playerMovement->ToggleDebug();
            }
        }
    }
}

void RunningState::ReadNetworkPackets() {
    while (!networkData->incomingInput.IsEmpty()) {
        auto data = networkData->incomingInput.Pop();
        UpdatePlayerMovement(GetPlayerObjectFromId(data.first), data.second);
    }
}

void RunningState::OnExit() {
    serverBase->ClearPacketHandlers();
    world->ClearAndErase();
    physics->Clear();
    playerObjects.clear();

    shouldClose.store(true);
    networkThread->join();
}


void RunningState::Update(float dt) {

    while (numPlayersLoaded < playerInfo.size()) {
        ReadNetworkFunctions();
        ReadNetworkPackets();
    }
    ReadNetworkFunctions();
    ReadNetworkPackets();
    UpdatePlayerAnimations();
    if (levelManager->GetCountdown() == COUNTDOWN_MAX) {//i.e only once, do this so player positions are correct.
        Tick(dt);
    }

    if (!levelManager->UpdateCountdown(dt)) {
        return;
    }

    world->UpdateWorld(dt);
    physics->Update(dt);
    Tick(dt);

    levelManager->UpdateTimer(dt);
}

void RunningState::LoadLevel() {
    BuildLevel("adamtest");
    // Change the order of these functions and the program will explode.
    CreatePlayers();
    CreateGrapples();
    AddTriggersToLevel();

    physics->InitialiseSortAndSweepStructs();
}

void RunningState::Tick(float dt) {
    packetTimer -= dt;
    if (packetTimer < 0) {
        SendWorldToClient();
        packetTimer += Replicated::SERVERHERTZ;
    }
}

void RunningState::SendWorldToClient() {
    for (auto i = world->GetNetworkIteratorStart(); i < world->GetNetworkIteratorEnd(); i++) {
        FullPacket newPacket;
        if ((*i)->WritePacket(newPacket, false, sceneSnapshotId)) {
            networkData->outgoingState.Push(newPacket);
        }
    }

    sceneSnapshotId++;
}

void RunningState::CreateGrapples() {
    for (int i = 0; i < Replicated::PLAYERCOUNT; i++) {
        auto g = new GameObject();
        replicated->AddGrapplesToWorld(g, *world, i);
        SetNetworkActive(g, false);
        grapples[i] = g;
    }
}

void RunningState::AssignPlayer(int peerId, GameObject* object) {
    FunctionData data{};
    DataHandler handler(&data);
    handler.Pack(object->GetNetworkObject()->GetNetworkId());
    networkData->outgoingFunctions.Push(std::make_pair(peerId, FunctionPacket(Replicated::AssignPlayer, &data)));
}

void RunningState::GrappleStart(GameObject* player, Vector3 direction) {
    int playerId = GetIdFromPlayerObject(player);
    const Vector3 &pos = player->GetTransform().GetPosition();
    grapples[playerId]->GetTransform().SetPosition(pos);
    auto lookDirection = Quaternion(Matrix4::BuildViewMatrix(pos, pos + direction, Vector3(0, 1, 0)).Inverse());
    auto flatRotation = Matrix3::Rotation(-90, Vector3(1,0,0));
    grapples[playerId]->GetTransform().SetOrientation(Quaternion(Matrix3(lookDirection) * flatRotation).Normalised());
    SetNetworkActive(grapples[playerId], true);
}

void RunningState::GrappleUpdate(GameObject* player, Vector3 position) {
    int playerId = GetIdFromPlayerObject(player);
    grapples[playerId]->GetTransform().SetPosition(position);
}

void RunningState::GrappleEnd(GameObject* player) {
    int playerId = GetIdFromPlayerObject(player);
    SetNetworkActive(grapples[playerId], false);
}

void RunningState::SetPlayerAnimation(Replicated::PlayerAnimationStates state, GameObject* object) {
    int id = object->GetNetworkObject()->GetNetworkId();
    if (playerAnimationInfo[id] == state)return;
    playerAnimationInfo[id] = state;
    SendPlayerAnimationCall(state, object);
}

void RunningState::SendPlayerAnimationCall(Replicated::PlayerAnimationStates state, GameObject* object) {
    FunctionData data{};
    DataHandler handler(&data);
    Replicated::RemoteAnimationData animData(object->GetNetworkObject()->GetNetworkId(), state);
    handler.Pack(animData);
    networkData->outgoingGlobalFunctions.Push(FunctionPacket(Replicated::Player_Animation_Call, &data));
}

void RunningState::CreatePlayers() {
    // For each player in the game create a player for them.
    for (auto index = 0; index < Replicated::PLAYERCOUNT; index++) {
        playerAnimationInfo[index] = Replicated::PlayerAnimationStates::IDLE; //players start as idle
        auto player = new GameObject("player");
        replicated->CreatePlayer(player, *world);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume(), new PhysicsMaterial()));
        player->GetPhysicsObject()->SetInverseMass(2.0f);
        player->GetPhysicsObject()->InitSphereInertia();
        player->GetPhysicsObject()->SetPhysMat(physics->GetPhysMat("Player"));
        player->GetPhysicsObject()->SetLayer(PLAYER_LAYER);
        player->SetTag(Tag::PLAYER);
        player->GetTransform().SetPosition(currentLevelStartPos);
        auto playerComponent = new PlayerMovement(player, world.get());
        playerComponent->GrappleStart.connect<&RunningState::GrappleStart>(this);
        playerComponent->GrappleEnd.connect<&RunningState::GrappleEnd>(this);
        playerComponent->GrappleUpdate.connect<&RunningState::GrappleUpdate>(this);
        player->AddComponent((Component*)playerComponent);

        playerObjects[index] = player;
    }
}

void RunningState::StartTriggerVolFunc(int id){
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Stage_Start, nullptr)));
    levelManager->StartStageTimer();
}

void RunningState::EndTriggerVolFunc(int id){
    levelManager->EndStageTimer();
    int medal = levelManager->GetCurrentMedal();
    Vector4 medalColour = levelManager->GetCurrentMedalColour();
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    handler.Pack(medal);
    handler.Pack(medalColour);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::EndReached, &data)));
}

void RunningState::DeathTriggerVolFunc(int id){
    CancelGrapple(id);
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Death_Event, nullptr)));
}

void RunningState::DeathTriggerVolEndFunc(int id){
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Death_Event_End, nullptr)));
}

void RunningState::AddTriggersToLevel(){
    for (auto& triggerVec : triggersVector){
        auto trigger = new TriggerVolumeObject(triggerVec.first, [this](GameObject* player) { return GetIdFromPlayerObject(player); });

        Vector4 colour = Vector4();
        Vector3 tempSize = Vector3();
        SortTriggerInfoByType(triggerVec.first, (Vector4&)colour, (Vector3&)tempSize);

        replicated->AddTriggerVolumeToWorld(tempSize, trigger, *world);
        trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(),
                                                    trigger->GetBoundingVolume(),
                                                    physics->GetPhysMat("Default")));
        trigger->GetPhysicsObject()->InitCubeInertia();
        trigger->GetPhysicsObject()->SetInverseMass(0.0f);
        trigger->GetTransform().SetPosition(triggerVec.second);
        trigger->GetPhysicsObject()->SetIsTriggerVolume(true);
        trigger->GetPhysicsObject()->SetLayer(TRIGGER_LAYER);

        trigger->TriggerSinkEndVol.connect<&RunningState::EndTriggerVolFunc>(this);
        trigger->TriggerSinkDeathVol.connect<&RunningState::DeathTriggerVolFunc>(this);
        trigger->TriggerSinkDeathVolEnd.connect<&RunningState::DeathTriggerVolEndFunc>(this);
        trigger->TriggerSinkStartVol.connect<&RunningState::StartTriggerVolFunc>(this);

        Debug::DrawAABBLines(triggerVec.second, tempSize, colour, 1000.0f);
    }
}


void RunningState::SortTriggerInfoByType(TriggerVolumeObject::TriggerType &triggerType, Vector4 &colour, Vector3 &dimensions) {
    switch (triggerType) {
        case TriggerVolumeObject::TriggerType::Start:
            colour = {1, 1, 1, 1};
            dimensions = Vector3(10, 10, 10);
            break;
        case TriggerVolumeObject::TriggerType::End:
            colour = {0, 1, 0, 1};
            dimensions = Vector3(10, 10, 10);
            break;
        case TriggerVolumeObject::TriggerType::Death:
            colour = {1, 0, 0, 1};
            dimensions = Vector3(2000, 10, 2000);
            break;
        case TriggerVolumeObject::TriggerType::CheckPoint:
            colour = {1, 0.4f, 1, 1};
            dimensions = Vector3(10, 10, 10);
            break;
        default:
            colour = {0, 0, 0, 1};
            dimensions = Vector3(10, 10, 10);
            break;
    }
}

void RunningState::UpdatePlayerAnimations() {
    for (std::pair<int,GameObject*> playerObject : playerObjects){
        PlayerMovement* playerMovement;
        if (playerObject.second->TryGetComponent<PlayerMovement>(playerMovement)) {
            PlayerMovement::PlayerAnimationCallData data = playerMovement->playerAnimationCallData;
            if (data.isGrappling || (data.inAir && !data.isFalling)) {
                SetPlayerAnimation(Replicated::JUMP, playerObject.second);
                continue;
            }
            if (data.inAir) {
                SetPlayerAnimation(Replicated::FALLING, playerObject.second);
                continue;
            }
            if (!data.hasInput) {
                SetPlayerAnimation(Replicated::IDLE, playerObject.second);
                continue;
            }
            if (data.backwards) {
                SetPlayerAnimation(Replicated::RUNNING_BACK, playerObject.second);
                continue;
            }
            if (data.strafe ==0) {
                SetPlayerAnimation(Replicated::RUNNING_FORWARD, playerObject.second);
                continue;
            }
            if (data.strafe > 0) {
                SetPlayerAnimation(Replicated::RUNNING_RIGHT, playerObject.second);
                continue;
            }
            else {
                SetPlayerAnimation(Replicated::RUNNING_LEFT, playerObject.second);
                continue;
            }
        }
    }
}


void RunningState::UpdatePlayerMovement(GameObject* player, const InputPacket& inputInfo) {
    Vector3 lookDir = inputInfo.playerRotation.ToEuler();
    lookDir.x = 0;
    lookDir.z = 0;
    player->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(lookDir.x,lookDir.y,lookDir.z));
    //player->GetTransform().SetOrientation(inputInfo.playerRotation); //just in case we need it in the future
    auto rightAxis = inputInfo.rightAxis;

    PlayerMovement* playerMovement;
    if (player->TryGetComponent(playerMovement)) {
        playerMovement->UpdateInputs(rightAxis, inputInfo.playerDirection, inputInfo.playerRotation);
    } else {
        std::cerr << "Where tf player movement" << std::endl;
    }

    {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        handler.Pack(player->GetPhysicsObject()->GetLinearVelocity());
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Player_Velocity_Call, &data)));
    }

    if (playerMovement->cameraAnimationCalls.groundMovement > 0.05f) {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        handler.Pack(playerMovement->cameraAnimationCalls.groundMovement);
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Camera_GroundedMove, &data)));
        playerMovement->cameraAnimationCalls.groundMovement = 0.0f;
    }

    if (playerMovement->cameraAnimationCalls.jump) {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        handler.Pack(true);
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Camera_Jump, &data)));
        playerMovement->cameraAnimationCalls.jump = false;
    }

    if (playerMovement->cameraAnimationCalls.land > 0.0f) {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        handler.Pack(playerMovement->cameraAnimationCalls.land);
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Camera_Land, &data)));
        playerMovement->cameraAnimationCalls.land = 0.0f;
    }

    if (abs(playerMovement->cameraAnimationCalls.strafeSpeed) > 0.5f) {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        float speed = playerMovement->cameraAnimationCalls.strafeSpeed * (playerMovement->cameraAnimationCalls.isGrappling ? GRAPPLE_SWAY_MULTIPLIER:1.0f);
        handler.Pack(speed);
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Camera_Strafe, &data)));
    }
    if ( int state = playerMovement->cameraAnimationCalls.grapplingEvent != 0) {
        auto id = GetIdFromPlayerObject(player);
        FunctionData data;
        DataHandler handler(&data);
        handler.Pack(playerMovement->cameraAnimationCalls.grapplingEvent);
        networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket( Replicated::Grapple_Event , &data)));
        playerMovement->cameraAnimationCalls.grapplingEvent = 0;
    }
}

void RunningState::ApplyPlayerMovement() {

}

void RunningState::BuildLevel(const std::string &levelName)
{
    //TODO: REDO THIS FUNCTION WITH LEVELMANAGER
    std::cout << "Level: " << levelName << " being built\n";
    levelManager->TryReadLevel(levelName);

    SetTriggerTypePositions();

    auto plist = levelManager->GetLevelReader()->GetPrimitiveList();
    auto opList = levelManager->GetLevelReader()->GetOscillatorPList();
    auto harmOpList = levelManager->GetLevelReader()->GetHarmfulOscillatorPList();
    auto swingpList = levelManager->GetLevelReader()->GetSwingingPList();
    auto springList = levelManager->GetLevelReader()->GetSpringPList();

    for(auto& x: plist){
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(STATIC_LAYER);
    }

    for (auto& x : opList) {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(OSCILLATOR_LAYER);

        ObjectOscillator* oo = new ObjectOscillator(g,x->timePeriod,x->distance,x->direction,x->cooldown,x->waitDelay);
        g->AddComponent(oo);
    }

    for (auto& x : harmOpList) {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(OSCILLATOR_LAYER);

        ObjectOscillator* oo = new ObjectOscillator(g, x->timePeriod, x->distance, x->direction, x->cooldown, x->waitDelay);
        DamagingObstacle* dO = new DamagingObstacle(g);
        g->AddComponent(oo);
        g->AddComponent(dO);
    }
  
    for (auto& x : springList) {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(STATIC_LAYER);

        Spring* oo = new Spring(g,x->direction * x->force,x->activeTime,x->isContinuous,x->direction * x->continuousForce);
        g->AddComponent(oo);
    }

    for (auto& x : swingpList)
    {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(OSCILLATOR_LAYER);

        Swinging* swing = new Swinging(g, x->timePeriod, x->cooldown, x->waitDelay, x->radius, x->changeAxis, x->changeDirection);
        g->AddComponent(swing);
    }

    //SetTestSprings(); 
    //SetTestFloor();
}

void RunningState::SetTriggerTypePositions(){
    currentLevelStartPos = levelManager->GetLevelReader()->GetStartPosition();
    currentLevelEndPos = levelManager->GetLevelReader()->GetEndPosition();
    currentLevelDeathPos = levelManager->GetLevelReader()->GetDeathBoxPosition() - Vector3(0,50,0); // Alter this if the death plane is set too high.
    currentLevelCheckPointPositions = levelManager->GetLevelReader()->GetCheckPointPositions();

    triggersVector = {
            std::make_pair((TriggerVolumeObject::TriggerType::Start), currentLevelStartPos),
            std::make_pair((TriggerVolumeObject::TriggerType::End), currentLevelEndPos),
            std::make_pair((TriggerVolumeObject::TriggerType::Death), currentLevelDeathPos),
    };
    for (auto checkpoint : currentLevelCheckPointPositions) {
        triggersVector.emplace_back(std::make_pair((TriggerVolumeObject::TriggerType::CheckPoint), checkpoint));
    }
}

void RunningState::CancelGrapple(int id)
{
    auto player = GetPlayerObjectFromId(id);
    PlayerMovement* playerMovement;
    if (player->TryGetComponent(playerMovement)) {
        playerMovement->grappleProjectileInfo.SetActive(false);
        playerMovement->grappleProjectileInfo.travelDistance = 0;
        playerMovement->LeaveGrappleState();
    }
}

void RunningState::SetTestSprings() {
    for (int i = 0; i < 4; i++) {
        auto g = new GameObject("Spring");
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -40.0f));
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->AddComponent((Component*)(new Spring(g, Vector3(500.0f * pow(i, 5), 1000, 0), false)));
    }
    for (int i = 0; i < 4; i++) {
        auto g = new GameObject("Spring");
        replicated->AddSpringToLevel(g, *world, Vector3(-40.0f + 15.0f * i, -3.0f, -50.0f));
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->AddComponent((Component*)(new Spring(g, Vector3(0.0f, 1000.0f, 0), true, 1.0f, Vector3(0.5f * pow(i, 4), 0, 0))));
    }
}

void RunningState::SetTestFloor() {
    auto g2 = new GameObject();
    auto x = new PrimitiveGameObject();
    x->position = Vector3(0, -5, 0);
    x->colliderExtents = Vector3(200, 2, 200);
    x->dimensions = Vector3(200, 2, 200);

    replicated->AddBlockToLevel(g2, *world, x);
    g2->SetPhysicsObject(new PhysicsObject(&g2->GetTransform(), g2->GetBoundingVolume(), new PhysicsMaterial()));
    g2->GetPhysicsObject()->SetInverseMass(0.0f);
    AddTriggersToLevel();
}

void RunningState::SetNetworkActive(GameObject *g, bool isActive) {
    g->SetActive(isActive);
    FunctionData data;
    DataHandler handler(&data);
    if (!g->GetNetworkObject()) {
        std::cerr << "Can't set network active without network object" << std::endl;
        exit(666);
    }
    handler.Pack(g->GetNetworkObject()->GetNetworkId());
    handler.Pack(isActive);

    networkData->outgoingGlobalFunctions.Push(FunctionPacket(Replicated::RemoteClientCalls::SetNetworkActive, &data));
}
