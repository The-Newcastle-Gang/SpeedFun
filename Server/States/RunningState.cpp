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

    currentLevelDeathPos = {0,0,0};
}

RunningState::~RunningState() {

}

void RunningState::OnEnter() {
    //serverBase has been called before the network thread has been created.
    serverBase->CallRemoteAll(Replicated::RemoteClientCalls::LoadGame, nullptr);
    playerInfo = serverBase->GetPlayerInfo();
    sceneSnapshotId = 0;
    CreateNetworkThread();
    LoadLevel();
    world->StartWorld();

}

void RunningState::ThreadUpdate(GameServer* server, ServerNetworkData* networkData) {
    auto threadServer = ServerThread(server, networkData);

    while (server) {
        threadServer.Update();
    }
}

void RunningState::CreateNetworkThread() {
    GameServer* server = serverBase;
    // Again, make sure serverBase isn't used without confirmation.
    serverBase = nullptr;
    networkData = std::make_unique<ServerNetworkData>();
    networkThread = new std::thread(ThreadUpdate, server, networkData.get());
    networkThread->detach();
}

void RunningState::ReadNetworkFunctions() {
    while (!networkData->incomingFunctions.IsEmpty()) {
        auto data = networkData->incomingFunctions.Pop();
        if (data.second.functionId == Replicated::RemoteServerCalls::GameLoaded) {
            AssignPlayer(data.first, GetPlayerObjectFromId(data.first));
        }
        else if(data.second.functionId == Replicated::RemoteServerCalls::PlayerJump){
            auto player = GetPlayerObjectFromId(data.first);
            PlayerMovement* playerMovement;
            if (player->TryGetComponent(playerMovement)) {
                playerMovement->Jump();
            }
        } else if (data.second.functionId == Replicated::RemoteServerCalls::PlayerGrapple) {
            auto player = GetPlayerObjectFromId(data.first);
            PlayerMovement* playerMovement;
            if (player->TryGetComponent(playerMovement)) {
                playerMovement->Grapple();
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
    world->ClearAndErase();
    physics->Clear();
    playerObjects.clear();
}


void RunningState::Update(float dt) {
    ReadNetworkFunctions();
    ReadNetworkPackets();
    world->UpdateWorld(dt);
    physics->Update(dt);
    Tick(dt);
}

void RunningState::LoadLevel() {
    BuildLevel("newTest");
    AddTriggersToLevel();
    CreatePlayers();
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

void RunningState::CreatePlayers() {
    // For each player in the game create a player for them.
    for (auto& pair : playerInfo) {
        auto player = new GameObject("player");
        replicated->CreatePlayer(player, *world);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume(), new PhysicsMaterial()));
        player->GetPhysicsObject()->SetInverseMass(2.0f);
        player->GetPhysicsObject()->InitSphereInertia();
        player->GetPhysicsObject()->SetPhysMat(physics->GetPhysMat("Player"));
        player->GetPhysicsObject()->SetLayer(PLAYER_LAYER);
        player->SetTag(Tag::PLAYER);

        //TODO: clean up
//        player->GetTransform().SetPosition(Vector3(0,0,0));
        player->GetTransform().SetPosition(currentLevelStartPos);
        player->AddComponent((Component*)(new PlayerMovement(player, world.get())));

        playerObjects[pair.first] = player;
    }
}

void RunningState::AddTriggersToLevel(){
    for (auto& triggerVec : triggersVector){
        auto trigger = new TriggerVolumeObject(triggerVec.first);

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

void RunningState::UpdatePlayerMovement(GameObject* player, const InputPacket& inputInfo) {

    player->GetTransform().SetOrientation(inputInfo.playerRotation);
    auto rightAxis = inputInfo.rightAxis;

    PlayerMovement* playerMovement;
    if (player->TryGetComponent(playerMovement)) {
        playerMovement->UpdateInputs(rightAxis, inputInfo.playerDirection, inputInfo.playerRotation);
    } else {
        std::cerr << "Where tf player movement" << std::endl;
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
    std::cout << "Level: " << levelName << " being built\n";
    levelReader = new LevelReader();
    if (!levelReader->HasReadLevel(levelName + ".json"))
    {
        std::cerr << "No file available. Check " + Assets::LEVELDIR << std::endl;
        return;
    }

    SetTriggerTypePositions();

    auto plist = levelReader->GetPrimitiveList();
    auto opList = levelReader->GetOscillatorPList();
    auto harmOpList = levelReader->GetHarmfulOscillatorPList();

    for(auto& x: plist){
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(STATIC_LAYER);
    }

    for (auto x : opList) {
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
        g->GetPhysicsObject()->SetLayer(DEFAULT_LAYER);

        ObjectOscillator* oo = new ObjectOscillator(g,x->timePeriod,x->distance,x->direction,x->cooldown,x->waitDelay);
        g->AddComponent(oo);
    }

    for (auto x : harmOpList) {
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

    //SetTestSprings();
    SetTestFloor();
    SetRayEnemy();
}

void RunningState::SetTriggerTypePositions(){
    currentLevelStartPos = levelReader->GetStartPosition();
    currentLevelEndPos = levelReader->GetEndPosition();
    currentLevelDeathPos = levelReader->GetDeathBoxPosition() - Vector3(0,50,0); // Alter this if the death plane is set too high.
    currentLevelCheckPointPositions = levelReader->GetCheckPointPositions();

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

void RunningState::SetRayEnemy() {

    auto rayenemyFollow = new GameObject();
    auto x = new PrimitiveGameObject();
    x->position = Vector3(-80, 6, -7);
    x->colliderExtents = Vector3(1, 1, 1);
    x->dimensions = Vector3(1, 1, 1);
    x->shouldNetwork = true;
    replicated->AddBlockToLevel(rayenemyFollow, *world, x);

    rayenemyFollow->SetPhysicsObject(new PhysicsObject(&rayenemyFollow->GetTransform(), rayenemyFollow->GetBoundingVolume(), new PhysicsMaterial()));
    rayenemyFollow->GetPhysicsObject()->SetInverseMass(0.0f);
    RayEnemyFollow* followComp = new RayEnemyFollow(rayenemyFollow);
    rayenemyFollow->AddComponent(followComp);

    auto rayenemyShoot = new GameObject();
    replicated->AddTriggerVolumeToWorld(Vector3(10, 10, 10), rayenemyShoot, *world);
    rayenemyShoot->SetPhysicsObject(new PhysicsObject(&rayenemyShoot->GetTransform(), rayenemyShoot->GetBoundingVolume(), physics->GetPhysMat("Default")));
    rayenemyShoot->GetPhysicsObject()->SetInverseMass(0.0f);
    rayenemyShoot->GetTransform().SetPosition(Vector3(-80, 6, -7));
    rayenemyShoot->GetPhysicsObject()->SetIsTriggerVolume(true);
    RayEnemyShoot* shootComp = new RayEnemyShoot(rayenemyShoot);
    rayenemyShoot->AddComponent(shootComp);
    Debug::DrawAABBLines(Vector3(-80, 6, -7), Vector3(5, 5, 5), Debug::MAGENTA, 1000.0f);

    shootComp->SetFollowComponent(followComp);
}
