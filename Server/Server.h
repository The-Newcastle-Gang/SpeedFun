//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_SERVER_H
#define CSC8503_SERVER_H

#include "NetworkBase.h"
#include "GameServer.h"
#include "Replicated.h"
#include "PacketTypes.h"
#include "entt.hpp"
#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "NetworkObject.h"
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "BusyState.h"
#include "RunningState.h"
#include "WaitingState.h"

#include <iostream>
#include <memory>

using namespace NCL;
using namespace CSC8503;

class Server : public PacketReceiver {
public:
    Server();
    ~Server();
//    void ServerInit();
    void UpdateServer(float dt);
    void ReceivePacket(int type, GamePacket *payload, int source) override;

private:
    std::unique_ptr<StateMachine> stateManager;
    std::unique_ptr<GameServer> serverBase;
    std::unique_ptr<GameWorld> world;
    std::unique_ptr<PhysicsSystem> physics;
    std::unique_ptr<Replicated> replicated;

    float packetTimer;
    int sceneSnapshotId;

    void RegisterPackets();
//    void InitGame();
//    void Tick(float dt);
//    void SendWorldToClient();
//    void CreateServerObject(GameObject *g);
//    void CreatePlayers();
//    GameObject* GetPlayerFromPeerId(int peerId);
//
//    void AssignPlayer(int peerId);

//    void SendFunction(int peerId, int functionId, FunctionData *d);

    void InitStateMachine();
};


#endif //CSC8503_SERVER_H
