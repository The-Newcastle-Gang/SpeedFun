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
        AssignPlayer,
        LoadGame,
    };

    enum RemoteServerCalls {
        StartGame = 40,
        GameLoaded = 41,
    };

    Replicated();
    void CreateObject(GameObject *g, GameWorld& world);
    void CreatePlayer(GameObject *g, GameWorld& world);
    constexpr static int PLAYERCOUNT = 4;
    constexpr static float SERVERHERTZ = 1.0f / 60.0f;

};


#endif //CSC8503_REPLICATED_H
