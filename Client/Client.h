//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_CLIENT_H
#define CSC8503_CLIENT_H

#include "NetworkBase.h"
#include "GameClient.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Replicated.h"
#include "PacketTypes.h"
#include "Quaternion.h"

#include <iostream>
#include <memory>

using namespace NCL;
using namespace CSC8503;

class Client : public PacketReceiver {
public:
    void InitClient();
    std::string GetAddress();
    void UpdateClient(float dt);
    void ReceivePacket(int type, GamePacket* payload, int source) override;
private:
    std::unique_ptr<GameClient> baseClient;
    std::unique_ptr<GameWorld> world;
    std::unique_ptr<GameTechRenderer> renderer;

    std::vector<NetworkObject*> networkObjects;

    void InitNetworking();
    void InitGame();
    void SendInputData();
    void InitCamera();
    void InitialiseAssets();

    void RegisterPackets();
};


#endif //CSC8503_CLIENT_H
