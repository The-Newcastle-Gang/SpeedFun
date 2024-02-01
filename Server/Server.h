//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_SERVER_H
#define CSC8503_SERVER_H

#include "NetworkBase.h"
#include "GameServer.h"
#include "Replicated.h"

#include <iostream>
#include <memory>

using namespace NCL;
using namespace CSC8503;

class Server {
    std::unique_ptr<GameServer> serverBase;
public:
    void ServerInit();
    void UpdateServer(float dt);
};


#endif //CSC8503_SERVER_H
