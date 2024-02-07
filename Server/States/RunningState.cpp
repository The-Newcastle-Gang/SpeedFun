#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;

RunningState::RunningState(GameServer* pBaseServer) : State()
{
    serverBase = pBaseServer;
    replicated = std::make_unique<Replicated>();
    world = std::make_unique<GameWorld>();
    physics = std::make_unique<PhysicsSystem>(*world);

}

RunningState::~RunningState() {

}

void RunningState::OnEnter() {
    serverBase->CallRemoteAll(Replicated::RemoteClientCalls::LoadGame, nullptr);
    sceneSnapshotId = 0;
    LoadLevel();
}

void RunningState::OnExit() {
    world->ClearAndErase();
    physics->Clear();
    playerObjects.clear();
}


void RunningState::Update(float dt) {
    world->UpdateWorld(dt);
    physics->Update(dt);
    Tick(dt);
}

void RunningState::LoadLevel() {
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
    // Test swapping this with iterators instead of lambda callback for performance.
    world->OperateOnContents([this](GameObject* obj) {
        if (!obj->IsActive()) {
            return;
        }

        auto netObj = obj->GetNetworkObject();
        if (!netObj) {
            return;
        }

        GamePacket* newPacket = nullptr;
        if (netObj->WritePacket(&newPacket, false, sceneSnapshotId)) {
            serverBase->SendGlobalPacket(*newPacket);
        }
        delete newPacket;
    });

    dia.gameTimer->Tick();

    if (dia.gameTimer->GetTimeDeltaSeconds() > 0.5f) {
        std::cout << "Delay in packets sent: " << dia.gameTimer->GetTimeDeltaSeconds() << std::endl;
    }
    dia.packetCount++;
    sceneSnapshotId++;
}

void RunningState::AssignPlayer(int peerId, GameObject* object) {
    FunctionData data{};
    DataHandler handler(&data);
    handler.Pack(object->GetNetworkObject()->GetNetworkId());
    serverBase->CallRemote(Replicated::AssignPlayer, &data, peerId);
}

void RunningState::CreatePlayers() {
    // For each player in the game create a player for them.
    for (auto pair : serverBase->GetPlayerInfo()) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
        player->GetPhysicsObject()->InitSphereInertia();
        playerObjects[pair.first] = player;
    }
}

void RunningState::ReceivePacket(int type, GamePacket *payload, int source) {
    switch (type) {
        case Received_State: {
            auto packet = reinterpret_cast<InputPacket*>(payload);
            GameObject* player = GetPlayerObjectFromId(source);
            player->GetTransform().SetOrientation(packet->playerRotation);

        } break;
        case Function: {
            auto packet = reinterpret_cast<FunctionPacket*>(payload);
            if (packet->functionId == Replicated::RemoteServerCalls::GameLoaded) {
                AssignPlayer(source, GetPlayerObjectFromId(source));
            }
        } break;
    }
}
