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
#include "RenderObject.h"
#include "StateMachine.h"
#include "State.h"
#include "GameplayState.h"
#include "LoadingState.h"
#include "MenuState.h"
#include "SafeQueue.h"
#include "ClientThread.h"
#include "NetworkObject.h"
#include "enet.h"
#include "ClientNetworkData.h"

#include <iostream>
#include <memory>
#include <array>



using namespace NCL;
using namespace CSC8503;

class Client{
public:
    Client();
    void InitClient();
    std::string GetAddress();
    void Update(float dt);

private:

    std::unique_ptr<StateMachine> stateManager;
    std::unique_ptr<GameWorld> world;
    std::unique_ptr<GameTechRenderer> renderer;
    std::unique_ptr<Replicated> replicated;
    std::unique_ptr<ClientNetworkData> networkData;
    std::unique_ptr<GameClient> baseClient;
    void InitStateManager();
};


#endif //CSC8503_CLIENT_H
