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

    sceneSnapshotId++;
}

void RunningState::AssignPlayer() {}

void RunningState::CreatePlayers() {
    for (int i=0; i<Replicated::PLAYERCOUNT; i++) {
        auto player = new GameObject();
        replicated->CreatePlayer(player, *world);

        player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
        player->GetPhysicsObject()->InitSphereInertia();

    }
}

void RunningState::ReceivePacket(int type, GamePacket *payload, int source) {
    switch (type) {
        case Received_State: {
            auto packet = reinterpret_cast<InputPacket*>(payload);
            GameObject* player = GetPlayerFromPeerId(source);
            player->GetTransform().SetOrientation(packet->playerRotation);
            break;
        }
    }
}
