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
    BuildLevel("debuglvl");
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
        replicated->AddTriggerVolumeToWorld(Vector3(10,10,10), trigger, *world);
        trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(),
                                                    trigger->GetBoundingVolume(),
                                                    physics->GetPhysMat("Default")));
        trigger->GetPhysicsObject()->InitCubeInertia();
        trigger->GetPhysicsObject()->SetInverseMass(0.0f);
        trigger->GetTransform().SetPosition(triggerVec.second);
        trigger->GetPhysicsObject()->SetIsTriggerVolume(true);

        Vector4 colour = Vector4();
        switch (triggerVec.first){
            case TriggerVolumeObject::TriggerType::Start:
                colour = {1,1,1,1};
                break;
            case TriggerVolumeObject::TriggerType::End:
                colour = {0,1,0,1};
                break;
            case TriggerVolumeObject::TriggerType::Death:
                colour = {1,0,0,1};
                break;
            case TriggerVolumeObject::TriggerType::CheckPoint:
                colour = {1,0.4f,1,1};
                break;
            default:
                colour = {0,0,0,1};
                break;
        }
        Debug::DrawAABBLines(triggerVec.second, Vector3(5,5,5),
                             colour, 1000.0f);
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
            std::make_pair((TriggerVolumeObject::TriggerType::Start), currentLevelStartPos),
            std::make_pair((TriggerVolumeObject::TriggerType::End), currentLevelEndPos),
            std::make_pair((TriggerVolumeObject::TriggerType::Death), currentLevelDeathPos),
            std::make_pair((TriggerVolumeObject::TriggerType::CheckPoint), Vector3(-62.0f,7.0f,-15.0f)),
            std::make_pair((TriggerVolumeObject::TriggerType::CheckPoint), Vector3(53.0f,7.0f,-15.0f)),
            std::make_pair((TriggerVolumeObject::TriggerType::CheckPoint), Vector3(122.0f,7.0f,-15.0f)),
    };

    auto plist = levelReader->GetPrimitiveList();
    for(auto x: plist){
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
    }

    // TEST SWINGING OBJECT ON THE SERVER
    auto swingingObject = new GameObject("Swinging_Object");
    replicated->AddSwingingBlock(swingingObject, *world);
    swingingObject->SetPhysicsObject(new PhysicsObject(&swingingObject->GetTransform(), swingingObject->GetBoundingVolume(), new PhysicsMaterial()));
    swingingObject->GetPhysicsObject()->SetInverseMass(0.0f);
    swingingObject->AddComponent((Component*)(new SwingingObject(swingingObject)));
}
