#include "RunningState.h"
#include "RunningState.h"
#include "RunningState.h"
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
    WaitUntilClientsInGameplay(); //so we dont send the Load_Level packet to MenuState

    MoveToNewLevel(selectedLevel);
}

void RunningState::SendLevelToClients(int level) {
    FunctionData data{};
    DataHandler handler(&data);
    handler.Pack(level);
    serverBase->CallRemoteAll(Replicated::RemoteClientCalls::Load_Level, &data);
}

void RunningState::WaitUntilClientsInGameplay() {
    while (numPlayersInGameplayState < serverBase->GetPlayerInfo().size()) {
        ReadNetworkFunctions();
    }
}

void RunningState::MoveToNewLevel(int level) { //we cant call this mid-update as it gets rid of stuff, needs to be done after update;
    isGameInProgress = false;
    ResetLevelInfo();
    world->ClearAndErase(); //free the memory too so we dont leak
    physics->Clear();
    SendLevelToClients(level);
    LoadLevel(level);
    world->StartWorld();
    isGameInProgress = true;
}

void RunningState::ResetLevelInfo() {
    hasAllPlayersFinished = false;
    isGameInProgress = false;
    numPlayersLoaded = 0;
    for (std::pair<int, bool> info : playersFinished) {
        playersFinished[info.first] = false;
    }
    levelManager->Reset();
    levelManager->GetLevelReader()->Clear();
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
        else if (data.second.functionId == Replicated::RemoteServerCalls::MenuToGameplay) {
            numPlayersInGameplayState++;
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

void RunningState::WaitForPlayersLoaded() {
    while (numPlayersLoaded < playerInfo.size()) {
        ReadNetworkFunctions();
        ReadNetworkPackets();
    }
}

void RunningState::Update(float dt) {
    if (shouldMoveToNewLevel) {
        MoveToNewLevel(levelManager->GetAndSetNextLevel());
        shouldMoveToNewLevel = false;
    }
    if (!isGameInProgress) {
        ReadNetworkFunctions();
        return;
    }
    WaitForPlayersLoaded();
    if (levelManager->GetCountdown() == COUNTDOWN_MAX) {//i.e only once, do this so player positions are correct.
        Tick(dt);
    }
    ReadNetworkFunctions();
    ReadNetworkPackets();
    UpdatePlayerAnimations();


    if (!levelManager->UpdateCountdown(dt)) {
        return;
    }

    world->UpdateWorld(dt);
    physics->Update(dt);
    Tick(dt);

    levelManager->UpdateTimer(dt);
}

void RunningState::LoadLevel(int level) {
    levelManager->SetCurrentLevel(level);
    BuildLevel(levelManager->GetLevelReader()->GetLevelName(level));
    CreatePlayers();
    AddTriggersToLevel();
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

void RunningState::AssignPlayer(int peerId, GameObject* object) {
    FunctionData data{};
    DataHandler handler(&data);
    handler.Pack(object->GetNetworkObject()->GetNetworkId());
    networkData->outgoingFunctions.Push(std::make_pair(peerId, FunctionPacket(Replicated::AssignPlayer, &data)));
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

int GetDirectionFromPlayerNumber(int num) {
    return (((num % 2) * 2) - 1); // 0 = -1, 1 = 1
}

int GetMagnitudeFromPlayerNumber(int num) {
    return num < 3 ? 1 : 3;
}

void RunningState::CreatePlayers() {
    // For each player in the game create a player for them.
    Vector3 startPos = currentLevelStartPos;
    float playerSeperation = 2.0f;
    int currentPlayer = 1;

    Vector3 thisPlayerStartPos;
    for (auto& pair : playerInfo) {
        thisPlayerStartPos = startPos + Vector3(0,0,1) * GetDirectionFromPlayerNumber(currentPlayer) * GetMagnitudeFromPlayerNumber(currentPlayer)* playerSeperation;
        playerAnimationInfo[pair.first] = Replicated::PlayerAnimationStates::IDLE; //players start as idle
        auto player = new GameObject("player");
        replicated->CreatePlayer(player, *world);
        currentPlayer++;

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume(), new PhysicsMaterial()));
        player->GetPhysicsObject()->SetInverseMass(2.0f);
        player->GetPhysicsObject()->InitSphereInertia();
        player->GetPhysicsObject()->SetPhysMat(physics->GetPhysMat("Player"));
        player->GetPhysicsObject()->SetLayer(PLAYER_LAYER);
        player->SetTag(Tag::PLAYER);
        player->GetTransform().SetPosition(thisPlayerStartPos);
        player->AddComponent((Component*)(new PlayerMovement(player, world.get())));

        playerObjects[pair.first] = player;
    }
}

void RunningState::ClearLevel()
{
    //physics->Clear();
    currentLevelCheckPointPositions.clear();
    currentLevelDeathPos = { 0, 0, 0 };
    currentLevelEndPos = { 0, 0, 0 };
    currentLevelStartPos = { 0, 0, 0 };
    triggersVector.clear();
    world->ClearAndErase();
}

void RunningState::StartTriggerVolFunc(int id){
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::Stage_Start, nullptr)));
    levelManager->StartStageTimer();
}

void RunningState::EndTriggerVolFunc(int id){
    playersFinished[id] = true;
    playerTimes[id] = levelManager->GetCurrentStageTime();

    int numPlayersFinished = 0;
    for (std::pair<int, bool> playerFinished : playersFinished) {
        numPlayersFinished += playerFinished.second ? 1 : 0;
    }

    if (numPlayersFinished == playerObjects.size()) {
        hasAllPlayersFinished = true;
    }
    if (!hasAllPlayersFinished)return;
    OnAllPlayersFinished();
    shouldMoveToNewLevel = true;
}

void RunningState::SendMedalToClient(int id) {
    int medal = levelManager->GetCurrentMedal();
    Vector4 medalColour = levelManager->GetCurrentMedalColour();
    FunctionData data;
    DataHandler handler(&data);
    handler.Pack(id);
    handler.Pack(medal);
    handler.Pack(medalColour);
    networkData->outgoingFunctions.Push(std::make_pair(id, FunctionPacket(Replicated::EndReached, &data)));
}

void RunningState::OnAllPlayersFinished()
{
    levelManager->EndStageTimer();
    networkData->outgoingGlobalFunctions.Push(FunctionPacket(Replicated::All_Players_Finished, nullptr));
    std::cout << "ALL PLAYERS DONE!!!!!!!\n";
}

void RunningState::DeathTriggerVolFunc(int id){
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

        Debug::DrawAABBLines(triggerVec.second, tempSize, colour, 10.0f);
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
        g->GetPhysicsObject()->SetLayer(DEFAULT_LAYER);

        ObjectOscillator* oo = new ObjectOscillator(g,x->timePeriod,x->distance,x->direction,x->cooldown,x->waitDelay);
        g->AddComponent(oo);
    }

    for (auto& x : harmOpList) {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(DEFAULT_LAYER);

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
        g->GetPhysicsObject()->SetLayer(DEFAULT_LAYER);

        Spring* oo = new Spring(g,x->direction * x->force,x->activeTime,x->isContinuous,x->direction * x->continuousForce);
        g->AddComponent(oo);
    }
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
