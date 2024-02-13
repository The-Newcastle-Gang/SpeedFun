#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;

RunningState::RunningState(GameServer* pBaseServer) : State() {
    // Don't use serverBase without talking to other members of the team.
    serverBase = pBaseServer;
    replicated = std::make_unique<Replicated>();
    world = std::make_unique<GameWorld>();
    physics = std::make_unique<PhysicsSystem>(*world);

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
    for (auto pair : playerInfo) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume(), physics->GetPhysMat("Default")));
        player->GetPhysicsObject()->InitSphereInertia();
        player->GetPhysicsObject()->SetInverseMass(10.0f);
        player->GetPhysicsObject()->SetPhysMat(physics->GetPhysMat("Player"));

        //TODO: clean up
        player->GetTransform().SetPosition(currentLevelStartPos + Vector3(0,10,0));
        auto component = new PlayerPhysComponent(player, world.get());
        player->AddComponent((Component*)component);

        playerObjects[pair.first] = player;
    }
}

void RunningState::UpdatePlayerMovement(GameObject* player, const InputPacket& inputInfo) {

    player->GetTransform().SetOrientation(inputInfo.playerRotation);

    player->GetPhysicsObject()->AddForce(inputInfo.fwdAxis  *inputInfo.playerDirection.y * 100);
    player->GetPhysicsObject()->AddForce(inputInfo.rightAxis *inputInfo.playerDirection.x * 100);

    if(inputInfo.jumpInput == 1.0f){
        player->GetPhysicsObject()->AddForce(Vector3{0,1,0} * 100);
    } else{
//        player->GetPhysicsObject()->ClearForces();
    }

    std::cout << "player forces:" << player->GetPhysicsObject()->GetForce() << std::endl;

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

    auto plist = levelReader->GetPrimitiveList();
    for(auto x: plist){
        auto g = new GameObject();
        replicated->AddBlockToLevel(g, *world, x);
        g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume(), new PhysicsMaterial()));
        g->GetPhysicsObject()->SetInverseMass(0.0f);
    }
}
