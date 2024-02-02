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

#include <iostream>
#include <memory>

using namespace NCL;
using namespace CSC8503;

class Server : public PacketReceiver {

public:
    void ServerInit();
    void UpdateServer(float dt);
    void ReceivePacket(int type, GamePacket* payload, int source);

private:
    std::unique_ptr<GameServer> serverBase;
    std::unique_ptr<GameWorld> world;
    std::unique_ptr<PhysicsSystem> physics;



    constexpr static int SERVERHERTZ = 30;
    float packetTimer;
    int sceneSnapshotId;

    void RegisterPackets();

    void InitGame();

    void Tick(float dt);

    void SendWorldToClient();

    void CreateServerObject(GameObject *g);
};


#endif //CSC8503_SERVER_H
