//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_REPLICATED_H
#define CSC8503_REPLICATED_H


#include "GameWorld.h"
#include "NetworkObject.h"
#include <vector>
#include <string>

using namespace NCL;
using namespace CSC8503;

class Replicated {
public:
    enum RemoteClientCalls {
        AssignPlayer
    };

    enum RemoteServerCalls {

    };
    Replicated();
    void CreateObject(GameObject *g, GameWorld& world);
    void CreatePlayer(GameObject *g);

    constexpr static int PLAYERCOUNT = 4;
    std::vector<NetworkObject*> networkObjects;

private:
    int networkIdCounter;

};


#endif //CSC8503_REPLICATED_H
