#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;

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
    BuildLevel("debuglvl");
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
        player->SetIsPlayerBool(true);

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
        replicated->AddTriggerVolumeToWorld(Vector3(10,10,10), trigger, *world);
        trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(),
                                                    trigger->GetBoundingVolume(),
                                                    physics->GetPhysMat("Default")));
        trigger->GetPhysicsObject()->InitCubeInertia();
        trigger->GetPhysicsObject()->SetInverseMass(0.0f);
        trigger->GetTransform().SetPosition(triggerVec.second);
        trigger->GetPhysicsObject()->SetIsTriggerVolume(true);
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
    currentLevelStartPos = levelReader->GetStartPosition();
    currentLevelEndPos = levelReader->GetEndPosition();
    currentLevelDeathPos = levelReader->GetDeathBoxPosition();
    triggersVector = {
            std::make_pair((TriggerVolumeObject::TriggerType)1, currentLevelStartPos),
            std::make_pair((TriggerVolumeObject::TriggerType)2, currentLevelEndPos),
            std::make_pair((TriggerVolumeObject::TriggerType)4, currentLevelDeathPos)
    };

    auto plist = levelReader->GetPrimitiveList();
    for(auto x: plist){
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
    }
    AddTriggersToLevel();
}
