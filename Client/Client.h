//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_REPLICATED_H
#define CSC8503_REPLICATED_H

#include "NetworkBase.h"
#include "GameClient.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Replicated.h"

#include <iostream>
#include <memory>

using namespace NCL;
using namespace CSC8503;

class Client {
private:
    std::unique_ptr<GameClient> baseClient;
public:
    void InitClient();
    std::string GetAddress();
    void UpdateClient(float dt);
};


#endif //CSC8503_REPLICATED_H
